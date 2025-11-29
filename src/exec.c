/**
 * exec.c
 *
 * Command execution engine for the shell.
 *
 * OVERVIEW:
 * Responsible for the actual execution of commands, including:
 * - Built-in commands
 * - External programs
 * - Pipes between commands
 * - I/O redirection
 * - Background processes
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "error.h"
#include "exec.h"

enum { READ_END, WRITE_END };

/**
 * exec_builtin - Execute built-in shell commands
 * @current_ctx: Shell context with parsed command
 *
 * Built-in commands are implemented into the shell itself rather than spawning
 * external programs. Only checks the first command (commands[0]), so built-ins
 * can't be piped from.
 *
 * Return: 0 on match, 1 on match, -1 on error
 */
// Returns -1 on error, 0 on no match, 1 on match
int exec_builtin(struct repl_ctx *current_ctx) {
  static const struct command_associations built_ins[NUM_OF_BUILTINS] = {
      {"cat", cat},
      {"cd", cd},
      {"cler", cler},
      {"exit", exit_builtin},
      {"help", help}};

  for (int i = 0; i < NUM_OF_BUILTINS; i++) {
    if (strcmp(current_ctx->commands[0][0], built_ins[i].command_name) == 0) {
      return built_ins[i].command_function(current_ctx);
    }
  }
  return 0;
}

/**
 * create-pipes - Set up pipes for command pipeline
 * @current_ctx: Shell context with command count
 *
 * Return: Array of pipe file descriptor pairs, NULL on error
 */
int (*create_pipes(struct repl_ctx *current_ctx))[2] {
  int(*pipe_fds)[2] =
      malloc((current_ctx->commands_count - 1) * sizeof(int[2]));
  if (!pipe_fds) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count - 1; i++) {
    /**
     * pipe() creates a unidirectional data channel.
     * pipe_fds[i][0] is the read end, pipe_fds[i][1] is the write end.
     * Data written to [1] can be read from [0]
     */
    if (pipe(pipe_fds[i]) == -1) {
      error_msg("Failed to create pipe", true);

      /* Clean up any previously created pipes on error */
      for (unsigned int j = 0; j < i; j++) {
        close(pipe_fds[j][READ_END]);
        close(pipe_fds[j][WRITE_END]);
      }

      free(pipe_fds);

      return NULL;
    }
  }
  return pipe_fds;
}

/**
 * exec - Execute command pipeline
 * @current_ctx: Shell context
 *
 * This does quite a bit: 
 * - Checks if first command is a builtin and executes it if so
 * - Create pipes if there are multiple commands
 * - Fork child process for each command
 * - Set up pipes and I/O redirection in child
 * - Close all pipe file descriptors in parent and child
 * - Execute program in child with execvp
 * - Wait for all children to finish in parent (unless background process)
 * 
 * Return: 0 on success, -1 on error
 */
int exec(struct repl_ctx *current_ctx) {
  /*
   * We do not need to execute an external program for builtin commands and we
   * do not support piping with builtin commands, so we can
   * return early if running a builtin
   */
  const int is_builtin = exec_builtin(current_ctx);

  if (is_builtin == -1) {
    return -1;
  }

  if (is_builtin) {
    return 0;
  }

  int(*pipe_fds)[2] = NULL;

  if (current_ctx->commands_count > 1) {
    pipe_fds = create_pipes(current_ctx);
    if (!pipe_fds) {
      return -1;
    }
  }

  pid_t pids[current_ctx->commands_count];
  int status = 0;

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    /* Create a new process by duplicating the current process */
    pids[i] = fork();

    if (pids[i] == -1) {
      error_msg("Failed to fork process", true);
      return -1;
    }

    // Child process
    if (pids[i] == 0) {
      /**
       * Create a new process group if this is going to be a background process.
       * This prevents SIGINT from killing the background job.
       */
      if (current_ctx->is_background_process) {
        setpgid(0, 0);
      }

      if (pipe_fds) {
        if (i > 0) {
          /**
           * The dup2 system call lets us overwrite one file descriptor to refer
           * to another. Here, we make STDIN_FILENO refer to the read end of the
           * pipe. After calling this, reading from STDIN_FILENO reads from the
           * pipe.
           */
          if (dup2(pipe_fds[i - 1][READ_END], STDIN_FILENO) == -1) {
            error_msg(dup2_fail_msg, true);

            if (close(pipe_fds[i - 1][READ_END])) {
              error_msg(close_fail_msg, true);
            }
          }
        }

        /**
         * If not the last command, redirect stdout to next pipe.
         * After calling this, writing to STDOUT_FILENO writes to the pipe.
         */
        if (i < current_ctx->commands_count - 1) {
          if (dup2(pipe_fds[i][WRITE_END], STDOUT_FILENO) == -1) {
            error_msg(dup2_fail_msg, true);

            if (close(pipe_fds[i][WRITE_END])) {
              error_msg(close_fail_msg, true);
            }
          }
        }

        /**
         * We have already duplicated the pipe fds we need to stdin/stdout, so
         * we can close them all in the child. It is especially important to
         * close all of the write ends so that the reading process knows that
         * we've finished writing.
         */
        for (unsigned int j = 0; j < current_ctx->commands_count - 1; j++) {
          if (close(pipe_fds[j][READ_END]) == -1) {
            error_msg(close_fail_msg, true);
          }

          if (close(pipe_fds[j][WRITE_END]) == -1) {
            error_msg(close_fail_msg, true);
          }
        }
      }

      /*
       * Handle I/O redirection. We assume we are redirecting to and from a file
       * as the POSIX specification does, though many shells (starting with
       * ksh93) allow for redirecting to and from sockets.
       */
      if (current_ctx->in_stream_name[i]) {
        int in_fd = open(current_ctx->in_stream_name[i], O_RDONLY);
        if (in_fd == -1) {
          error_msg(open_fail_msg, true);
          exit(EXIT_FAILURE);
        }

        /* Redirect stdin to the specified file. */
        if (dup2(in_fd, STDIN_FILENO) == -1) {
          error_msg(dup2_fail_msg, true);

          if (close(in_fd)) {
            error_msg(close_fail_msg, true);
          }
        }
      }

      if (current_ctx->out_stream_name[i]) {
        /* The stream type is either write only (for >) or append (for >>) */
        int out_fd =
            open(current_ctx->out_stream_name[i],
                 O_WRONLY | current_ctx->out_stream_type[i] | O_CREAT, 0644);
        if (out_fd == -1) {
          error_msg(open_fail_msg, true);
          exit(EXIT_FAILURE);
        }

        if (dup2(out_fd, STDOUT_FILENO) == -1) {
          error_msg(dup2_fail_msg, true);
        }

        if (close(out_fd)) {
          error_msg(close_fail_msg, true);
        }
      }

      /**
       * execvp() replaces the current process image with a new program. This
       * will only return if the function fails.
       */
      if (execvp(current_ctx->commands[i][0], current_ctx->commands[i]) == -1) {
        error_msg("Failed to execute process", true);
        exit(EXIT_FAILURE);
      }
    }
  }

  /*
   * The parent process doesn't need the pipe file descriptors since the
   * children have their own copies, so we close them all.
   */
  if (pipe_fds) {
    for (unsigned int i = 0; i < current_ctx->commands_count - 1; i++) {
      if (close(pipe_fds[i][READ_END])) {
        error_msg(close_fail_msg, true);
      }

      if (close(pipe_fds[i][WRITE_END])) {
        error_msg(close_fail_msg, true);
      }
    }

    free(pipe_fds);
  }

  /**
   * For foreground processes, we keep waiting until all child processes have
   * exited normally or been terminated by SIGINT/SIGSTP before returning. If a
   * process is stopped (Ctrl-Z), waitpid() returns but we do not exit the loop
   * because a stopped process is not finished.
   */
  if (!current_ctx->is_background_process) {
    for (unsigned int k = 0; k < current_ctx->commands_count; k++) {
      do {
        waitpid(pids[k], &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  }

  return 0;
}

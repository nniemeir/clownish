#include "exec.h"
#include "builtins.h"
#include "context.h"
#include "error.h"
#include "tease.h"

enum { READ_END, WRITE_END };

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

int (*create_pipes(struct repl_ctx *current_ctx))[2] {
  int(*pipe_fds)[2] =
      malloc((current_ctx->commands_count - 1) * sizeof(int[2]));
  if (!pipe_fds) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count - 1; i++) {
    if (pipe(pipe_fds[i]) == -1) {
      error_msg("Failed to create pipe", true);
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

int exec(struct repl_ctx *current_ctx) {
  const int is_builtin = exec_builtin(current_ctx);
  if (is_builtin == -1) {
    return 1;
  }
  if (is_builtin) {
    return 0;
  }

  int(*pipe_fds)[2] = NULL;

  if (current_ctx->commands_count > 1) {
    pipe_fds = create_pipes(current_ctx);
    if (!pipe_fds) {
      return 1;
    }
  }

  pid_t pids[current_ctx->commands_count];
  int status;
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    pids[i] = fork();
    if (pids[i] == -1) {
      error_msg("Failed to fork process", true);
      return 1;
    }

    // Child process
    if (pids[i] == 0) {
      if (current_ctx->is_background_process) {
        setpgid(0, 0);
      }
      if (pipe_fds) {
        if (i > 0) {
          if (dup2(pipe_fds[i - 1][READ_END], STDIN_FILENO) == -1) {
            error_msg(dup2_fail_msg, true);
            if (close(pipe_fds[i - 1][READ_END])) {
              error_msg(close_fail_msg, true);
            }
          }
        }

        if (i < current_ctx->commands_count - 1) {
          if (dup2(pipe_fds[i][WRITE_END], STDOUT_FILENO) == -1) {
            error_msg(dup2_fail_msg, true);
            if (close(pipe_fds[i][WRITE_END])) {
              error_msg(close_fail_msg, true);
            }
          }
        }

        for (unsigned int j = 0; j < current_ctx->commands_count - 1; j++) {
          if (close(pipe_fds[j][READ_END]) == -1) {
            error_msg(close_fail_msg, true);
          }
          if (close(pipe_fds[j][WRITE_END]) == -1) {
            error_msg(close_fail_msg, true);
          }
        }
      }
      if (current_ctx->in_stream_name[i]) {
        int in_fd = open(current_ctx->in_stream_name[i], O_RDONLY);
        if (in_fd == -1) {
          error_msg(open_fail_msg, true);
          exit(EXIT_FAILURE);
        } else {
          if (dup2(in_fd, STDIN_FILENO) == -1) {
            error_msg(dup2_fail_msg, true);
            if (close(in_fd)) {
              error_msg(close_fail_msg, true);
            }
          }
        }
      }

      if (current_ctx->out_stream_name[i]) {
        int out_fd =
            open(current_ctx->out_stream_name[i],
                 O_WRONLY | current_ctx->out_stream_type[i] | O_CREAT, 0644);
        if (out_fd == -1) {
          error_msg(open_fail_msg, true);
          exit(EXIT_FAILURE);
        } else {
          if (dup2(out_fd, STDOUT_FILENO) == -1) {
            error_msg(dup2_fail_msg, true);
          }
          if (close(out_fd)) {
            error_msg(close_fail_msg, true);
          }
        }
      }

      if (execvp(current_ctx->commands[i][0], current_ctx->commands[i]) == -1) {
        error_msg("Failed to execute process", true);
        exit(EXIT_FAILURE);
      }
    }
  }
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

  // Wait until all child processes have exited
  if (!current_ctx->is_background_process) {
    for (unsigned int k = 0; k < current_ctx->commands_count; k++) {
      do {
        waitpid(pids[k], &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  }
  return 0;
}
#include "exec.h"
#include "error.h"
#include "tease.h"
#include <unistd.h>

enum { READ_END, WRITE_END };

int cat(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    return 0;
  }

  const int random_num = rand() % 101;
  if (random_num <= 10) {
    printf(" /\\_/\\\n");
    printf("( o.o )\n");
    printf(" > ^ <\n");
    printf("You requested my presence %s?\n", current_ctx->user);
    return 1;
  }
  return 0;
}

int cd(struct repl_ctx *current_ctx) {
  if (!current_ctx->commands[0][1]) {
    current_ctx->commands[0][1] = current_ctx->home_dir;
  }

  if (chdir(current_ctx->commands[0][1]) == -1) {
    error_msg("Failed to change working directory", true);
    fprintf(stderr, blame_user_msg, current_ctx->user);
    return -1;
  }

  return 1;
}

int cler(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    return 0;
  }
  fprintf(stderr,
          "Perhaps you meant to type clear but "
          "made a typo in your haste, let's take a breather for a moment.\n");
  sleep(10);
  printf("Don't you feel better %s?\n", current_ctx->user);
  return 1;
}

int exit_builtin(struct repl_ctx *current_ctx) {
  current_ctx->receiving = 0;
  printf("Finally giving up, %s?\n", current_ctx->user);
  return 1;
}

int help(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    printf("cd - change directory\n");
    printf("exit - exit shell\n");
    printf("help - display this message\n");
    return 1;
  }
  printf("You aren't seriously asking me to hold your hand, are you %s?\n",
         current_ctx->user);
  return 1;
}

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
    pipe_fds = malloc((current_ctx->commands_count - 1) * sizeof(int[2]));
    if (!pipe_fds) {
      fprintf(stderr, malloc_fail_msg, program_name);
      return 1;
    }
  }

  if (pipe_fds) {
    for (unsigned int i = 0; i < current_ctx->commands_count - 1; i++) {
      if (pipe(pipe_fds[i]) == -1) {
        error_msg("Failed to create pipe", true);
        free(pipe_fds);
        return 1;
      }
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

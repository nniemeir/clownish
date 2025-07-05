#include "exec.h"
#include "error.h"
#include "tease.h"

int cat(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    return 0;
  }

  const int rd_num = rand() % (100 - 0 + 1) + 0;
  if (rd_num <= 10) {
    printf(" /\\_/\\\n");
    printf("( o.o )\n");
    printf(" > ^ <\n");
    printf("You requested my presence %s?\n", current_ctx->user);
    return 1;
  }
  return 0;
}

int cd(struct repl_ctx *current_ctx) {
  if (!current_ctx->command[1]) {
    current_ctx->command[1] = current_ctx->home_dir;
  }

  if (chdir(current_ctx->command[1]) == -1) {
    perror(program_name);
    fprintf(stderr, blame_user_msg, current_ctx->user);
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
    if (strcmp(current_ctx->command[0], built_ins[i].command_name) == 0) {
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

  pid_t pid;
  int status;
  pid = fork();
  if (pid == -1) {
    fprintf(stderr, "I couldn't fork the process.\n");
    return 1;
  }

  // Parent process
  if (pid > 0 && !current_ctx->is_background_process) {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  // Child process
  if (pid == 0) {
    if (current_ctx->is_background_process) {
      setpgid(0, 0);
    }

    if (current_ctx->in_stream_name[0] != '\0') {
      int in_fd = open(current_ctx->in_stream_name, O_RDONLY);
      if (in_fd == -1) {
        perror("open");
      } else {
        if (dup2(in_fd, fileno(stdin)) == -1) {
          perror("dup2");
          close(in_fd);
        }
      }
    }

    if (current_ctx->out_stream_name[0] != '\0') {
      int out_fd =
          open(current_ctx->out_stream_name,
               O_WRONLY | current_ctx->out_stream_type | O_CREAT, 0644);
      if (out_fd == -1) {
        perror("open");
      } else {
        if (dup2(out_fd, fileno(stdout)) == -1) {
          perror("dup2");
        }
        close(out_fd);
      }
    }

    if (execvp(current_ctx->command[0], current_ctx->command) == -1) {
      perror(program_name);
      exit(EXIT_FAILURE);
    }
  }
  return 0;
}

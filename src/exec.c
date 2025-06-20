#include "../include/exec.h"
#include "../include/tease.h"

int cat(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    return 0;
  }

  const int rd_num = rand() % (100 - 0 + 1) + 0;
  if (rd_num <= 10) {
    printf(" /\\_/\\\n");
    printf("( o.o )\n");
    printf(" > ^ <\n");
    return 1;
  }
  return 0;
}

int cd(struct repl_ctx *current_ctx) {
  if (!current_ctx->args[1]) {
    current_ctx->args[1] = current_ctx->home_dir;
  }

  if (chdir(current_ctx->args[1]) == -1) {
    perror("clowniSH");
    return -1;
  }

  return 1;
}

int exit_builtin(struct repl_ctx *current_ctx) {
  current_ctx->receiving = 0;
  return 1;
}

int help(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    printf("cd - change directory\n");
    printf("exit - exit shell\n");
    printf("help - display this message\n");
    return 1;
  }
  printf("You aren't seriously asking me to hold your hand, are you?\n");
  return 1;
}

// Returns -1 on error, 0 on no match, 1 on match
int exec_builtin(struct repl_ctx *current_ctx) {
  static const struct command_associations built_ins[NUM_OF_BUILTINS] = {
      {"cat", cat}, {"cd", cd}, {"exit", exit_builtin}, {"help", help}};
  for (int i = 0; i < NUM_OF_BUILTINS; i++) {
    if (strcmp(current_ctx->args[0], built_ins[i].command_name) == 0) {
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
    printf("clowniSH: Failed to fork process.\n");
    return 1;
  }

  int fd = -2;
  int copy_out;
  if (current_ctx->out_stream_name[0] != '\0') {
    fd = open(current_ctx->out_stream_name,
              O_WRONLY | current_ctx->out_stream_type | O_CREAT, 0644);
    copy_out = dup(fileno(stdout));
    dup2(fd, fileno(stdout));
  }

  // Parent process
  if (pid > 0 && !current_ctx->is_background_process) {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  if (fd != -2) {
    dup2(copy_out, fileno(stdout));
    close(copy_out);
  }

  // Child process
  if (pid == 0) {
    if (current_ctx->is_background_process) {
      setpgid(0, 0);
    }

    if (execvp(current_ctx->args[0], current_ctx->args) == -1) {
      printf("clowniSH: Program not found.\n");
    }

    if (fd != -2) {
      dup2(copy_out, fileno(stdout));
      close(copy_out);
    }

    return 0;
  }
  return 0;
}

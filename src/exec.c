#include "../include/exec.h"

int baffled(void) {
  printf("mosey - change directory\n");
  printf("scram - exit shell\n");
  printf("baffled - display this message\n");
  return 1;
}

int scram(int *receiving) {
  *receiving = 0;
  return 1;
}

int mosey(struct repl_ctx *current_ctx) {
  if (!current_ctx->args[1]) {
    current_ctx->args[1] = current_ctx->home_dir;
  }
  if (chdir(current_ctx->args[1]) == -1) {
    perror("clowniSH");
  }
  return 1;
}

// Returns -1 on error, 0 on no match, 1 on match
int exec_builtin(struct repl_ctx *current_ctx, int *receiving) {
  if (strcmp(current_ctx->args[0], "baffled") == 0) {
    return baffled();
  }
  if (strcmp(current_ctx->args[0], "scram") == 0) {
    return scram(receiving);
  }
  if (strcmp(current_ctx->args[0], "mosey") == 0) {
    return mosey(current_ctx);
  }
  return 0;
}

int exec(struct repl_ctx *current_ctx, int *receiving) {
  int is_builtin = exec_builtin(current_ctx, receiving);
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
  if (pid > 0 && !current_ctx->is_background_process) {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  if (fd != -2) {
    dup2(copy_out, fileno(stdout));
    close(copy_out);
  }
  return 0;
}

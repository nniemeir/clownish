#include "../include/launch.h"

int launch(char **args, int *args_count) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == -1) {
    printf("clowniSH: Failed to fork process.\n");
    return 1;
  }
  int background = 0;
  int fd = -2;
  int copy_out;
  if (strcmp(args[*args_count - 1], "&") == 0) {
    background = 1;
    fd = open("/dev/null", O_WRONLY);
    copy_out = dup(fileno(stdout));
    dup2(fd, fileno(stdout));
    args[*args_count - 1] = NULL;
  }
  if (pid == 0) {
    // Child process
    if (background) {
      setpgid(0, 0);
    }
    if (execvp(args[0], args) == -1) {
      printf("clowniSH: Failed to execute child process.\n");
    }
    if (fd != -2) {
      dup2(copy_out, fileno(stdout));
      close(copy_out);
    }
    return 0;
  }
  if (pid > 0 && !background) {
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

// Returns -1 on error, 0 on no match, 1 on match
int launch_builtin(char **args, int *receiving) {
  if (strcmp(args[0], "help") == 0) {
    printf("cd - change directory\n");
    printf("exit - exit shell\n");
    printf("help - display this message\n");
    return 1;
  }

  if (strcmp(args[0], "exit") == 0) {
    *receiving = 0;
    return 1;
  }

  if (strcmp(args[0], "cd") == 0) {
    if (!args[1]) {
      args[1] = getenv("HOME");
    }
    if (chdir(args[1]) == -1) {
      printf("clownish: cd encountered an error.");
    }
    return 1;
  }

  return 0;
}

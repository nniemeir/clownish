#include "../include/main.h"

const char *program_name = "clownish";
int log_to_file = 1;

char **split_input(char *line, int *args_count) {
  int buffer_size = TOKEN_BUFFER_SIZE;
  char **tokens = malloc(buffer_size * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "clownish: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOKEN_DELIM);
  while (token) {
    tokens[*args_count] = token;
    (*args_count)++;

    if (*args_count >= buffer_size) {
      buffer_size += TOKEN_BUFFER_SIZE;
      tokens = realloc(tokens, buffer_size * sizeof(char *));
      if (!tokens) {
        fprintf(stderr, "clownish: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOKEN_DELIM);
  }
  tokens[*args_count] = NULL;
  return tokens;
}

int launch(char **args, int *args_count) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == -1) {
    log_event(program_name, FATAL, "Failed to fork process.", log_to_file);
    return 0;
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
      log_event(program_name, FATAL, "Failed to execute child process",
                log_to_file);
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
  return 1;
}

// Returns -1 on error, 0 on no match, 1 on match
int launch_builtin(char **args) { return 0; }

int main(void) {
  char *input = NULL;
  char **args;
  size_t U_ARG_MAX = ARG_MAX;
  bool receiving = true;
  while (receiving) {
    printf("clowniSH$ ");
    input = NULL;
    if (getline(&input, &U_ARG_MAX, stdin) == -1) {
      receiving = false;
      break;
    }
    if (strcmp(input, "\n") == 0) {
      continue;
    }
    int args_count = 0;
    args = split_input(input, &args_count);
    if (strcmp(args[0], "exit") == 0) {
      receiving = false;
      break;
    }
    int command_was_builtin = launch_builtin(args);
    switch (command_was_builtin) {
    case -1:
      log_event(program_name, FATAL, "Failed to launch a built-in command",
                log_to_file);
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    case 1:
      continue;
    }
    if (!launch(args, &args_count)) {
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    }
  }
  free(input);
  free(args);
  exit(EXIT_SUCCESS);
}
#include "../include/main.h"
#include "../include/exec.h"
#include "../include/prompt.h"

char *init_history() {
  char *home_dir = getenv("HOME");
  if (!home_dir) {
    printf("clowniSH: Failed to resolve $HOME.\n");
    return NULL;
  }
  char *hist_file = malloc(PATH_MAX);
  snprintf(hist_file, PATH_MAX, "%s/.scribbles", home_dir);
  using_history();
  read_history(hist_file);
  return hist_file;
}

void close_history(char *hist_file) {
  write_history(hist_file);
  free(hist_file);
  clear_history();
}

int main(void) {
  char *hist_file = init_history();
  if (!hist_file) {
    exit(EXIT_FAILURE);
  }
  int receiving = 1;
  char *input = NULL;
  char **args;
  while (receiving) {
    int args_count = 0;
    if (prompt_loop(&args, &input, &args_count) == 1) {
      free(hist_file);
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    }
    if (input[0] == '\0') {
      free(input);
      free(args);
      continue;
    }
    int command_is_builtin = exec_builtin(args, &receiving);
    switch (command_is_builtin) {
    case -1:
      printf("clowniSH: Failed to execute built-in command.\n");
      free(hist_file);
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    case 1:
      free(input);
      free(args);
      continue;
    }
    if (exec(args, &args_count) == 1) {
      printf("clowniSH: Failed to execute command.\n");
      free(hist_file);
      free(input);
      free(args);
      continue;
    }
    free(input);
    free(args);
  }
  close_history(hist_file);
  exit(EXIT_SUCCESS);
}
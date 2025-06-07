#include "../include/main.h"
#include "../include/exec.h"
#include "../include/prompt.h"

int main(void) {
  char *home_dir = getenv("HOME");
  if (!home_dir) {
    printf("clowniSH: Failed to resolve $HOME.\n");
    exit(EXIT_FAILURE);
  }
  char *hist_path = malloc(PATH_MAX);
  snprintf(hist_path, PATH_MAX, "%s/.scribbles", home_dir);
  using_history();
  read_history(hist_path);
  int receiving = 1;
  char *input = NULL;
  char **args;
  while (receiving) {
    int args_count = 0;
    if (prompt_loop(&args, &input, &args_count) == 1) {
      free(hist_path);
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
      free(hist_path);
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
      free(hist_path);
      free(input);
      free(args);
      exit(EXIT_FAILURE);
      break;
    }
    free(input);
    free(args);
  }
  write_history(hist_path);
  free(hist_path);
  clear_history();
  exit(EXIT_SUCCESS);
}
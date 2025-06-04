#include "../include/main.h"
#include "../include/launch.h"
#include "../include/prompt.h"

int main(void) {
  char *input = NULL;
  char **args;
  using_history();
  int receiving = 1;
  while (receiving) {
    int args_count = 0;
    if (prompt_loop(&args, &input, &args_count) == 1) {
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    }
    if (strcmp(input, "\n") == 0) {
      continue;
    }
    int is_builtin = exec_builtin(args, &receiving);
    switch (is_builtin) {
    case -1:
      printf("clowniSH: Failed to execute built-in command.\n");
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    case 1:
      continue;
    }
    if (exec(args, &args_count) == 1) {
      printf("clowniSH: Failed to execute command.\n");
      free(input);
      free(args);
      exit(EXIT_FAILURE);
      break;
    }
  }
  clear_history();
  free(input);
  free(args);
  exit(EXIT_SUCCESS);
}
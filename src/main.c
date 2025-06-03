#include "../include/main.h"
#include "../include/launch.h"
#include "../include/parse.h"
#include <linux/limits.h>
#include <unistd.h>

static int PROMPT_MAX = _SC_HOST_NAME_MAX + _SC_LOGIN_NAME_MAX + PATH_MAX;

int construct_prompt(char **prompt) {
  char *username = getlogin();
  if (!username) {
    printf("clowniSH: Failed to get username.\n");
    return 1;
  }
  char hostname[_SC_HOST_NAME_MAX];
  gethostname(hostname, _SC_HOST_NAME_MAX);
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd))) {
    printf("clowniSH: Failed to get current working directory.\n");
    return 1;
  }
  snprintf(*prompt, PROMPT_MAX, "[%s@%s] %s ", username, hostname, cwd);
  return 0;
}

int prompt_loop(char ***args, char **input, int *args_count) {
  static size_t U_ARG_MAX = ARG_MAX;
  char *prompt = malloc(PROMPT_MAX);
  if (!prompt) {
    printf("clowniSH: Failed to allocate memory for prompt.\n");
    return 1;
  }
  if (construct_prompt(&prompt) == 1) {
    printf("clowniSH: Failed to construct prompt, using default.\n");
    prompt = "clowniSH$ ";
  }
  printf("%s", prompt);
  free(prompt);
  *input = NULL;
  if (getline(input, &U_ARG_MAX, stdin) == -1) {
    return 1;
  }
  if (strcmp(*input, "\n") == 0) {
    return 0;
  }
  *args_count = 0;
  *args = split_input(*input, args_count);
  if (!*args[0]) {
    return 1;
  }
  return 0;
}

int main(void) {
  char *input = NULL;
  char **args;
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
    int is_builtin = launch_builtin(args, &receiving);
    switch (is_builtin) {
    case -1:
      printf("clowniSH: Failed to execute built-in command.\n");
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    case 1:
      continue;
    }
    if (launch(args, &args_count) == 1) {
      printf("clowniSH: Failed to execute command.\n");
      free(input);
      free(args);
      exit(EXIT_FAILURE);
      break;
    }
  }
  free(input);
  free(args);
  exit(EXIT_SUCCESS);
}
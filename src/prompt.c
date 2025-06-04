#include "../include/prompt.h"
#include "../include/parse.h"

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
  char *prompt = malloc(PROMPT_MAX);
  if (!prompt) {
    printf("clowniSH: Failed to allocate memory for prompt.\n");
    return 1;
  }
  if (construct_prompt(&prompt) == 1) {
    printf("clowniSH: Failed to construct prompt, using default.\n");
    prompt = "clowniSH$ ";
  }
  *input = readline(prompt);
  free(prompt);
  add_history(*input);
  if (strcmp(*input, "\n") == 0) {
    return 0;
  }
  *args_count = 0;
  *args = tokenize_input(*input, args_count);
  if (!*args[0]) {
    return 1;
  }
  return 0;
}

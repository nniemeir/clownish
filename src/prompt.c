#include "../include/prompt.h"
#include "../include/parse.h"
#define WHT "\x1b[37m"
#define BLU "\x1b[33m"
#define RED "\x1b[31m"

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
  char *tcompressed_cwd = replace(cwd, getenv("HOME"), "~");
  snprintf(*prompt, PROMPT_MAX, "%s[%s@%s] %s%s%s ", RED, username, hostname,
           BLU, tcompressed_cwd, WHT);
  free(tcompressed_cwd);
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
    strcpy(prompt, "clowniSH$ ");
  }
  *input = readline(prompt);
  free(prompt);
  if (!input || *input[0] == '\0') {
    return 0;
  }
  add_history(*input);
  char *tilde_expanded = replace(*input, "~", getenv("HOME"));
  if (!tilde_expanded) {
    return 0;
  }
  char *envs_parsed = parse_envs(tilde_expanded);
  free(tilde_expanded);
  *args_count = 0;
  *args = tokenize_input(envs_parsed, args_count);
  if (!*args[0]) {
    return 1;
  }
  return 0;
}
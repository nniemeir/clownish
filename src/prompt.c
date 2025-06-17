#include "../include/prompt.h"
#include "../include/history.h"
#include "../include/parse.h"

int construct_prompt(char **prompt, char *home_dir) {
  char *username = getlogin();
  if (!username) {
    printf("clowniSH: Failed to get username.\n");
    return 1;
  }
  char hostname[_SC_HOST_NAME_MAX];
  gethostname(hostname, _SC_HOST_NAME_MAX);
  char *cwd = malloc(PATH_MAX);
  if (!getcwd(cwd, PATH_MAX)) {
    perror("clowniSH: ");
    return 1;
  }
  replace(&cwd, home_dir, "~");
  snprintf(*prompt, PROMPT_MAX, "%s[%s@%s] %s%s%s ", RED, username, hostname,
           YEL, cwd, WHT);
  free(cwd);
  return 0;
}

int prompt_loop(struct repl_ctx *current_ctx) {
  char *prompt = malloc(PROMPT_MAX);
  if (!prompt) {
    printf("clowniSH: Failed to allocate memory for prompt.\n");
    return 1;
  }
  if (construct_prompt(&prompt, current_ctx->home_dir) == 1) {
    printf("clowniSH: Failed to construct prompt, using default.\n");
    strcpy(prompt, "clowniSH$ ");
  }
  current_ctx->input = readline(prompt);
  free(prompt);
  if (!current_ctx->input || current_ctx->input[0] == '\0') {
    return 0;
  }
  add_history(current_ctx->input);
  current_ctx->args_count = 0;
  current_ctx->args =
      tokenize_input(current_ctx->input, &current_ctx->args_count);
  if (!current_ctx->args[0]) {
    return 1;
  }
  check_if_background(current_ctx);
  determine_out_stream(current_ctx);
  for (unsigned int i = 0; i < current_ctx->args_count; i++) {
    replace(&current_ctx->args[i], "~", current_ctx->home_dir);
    if (!parse_envs(&current_ctx->args[i])) {
      printf("clowniSH: Failed to parse environment variables.\n");
    }
  }
  return 0;
}
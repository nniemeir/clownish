#include "../include/prompt.h"
#include "../include/history.h"
#include "../include/parse.h"
#include "../include/stream.h"

int construct_prompt(char **prompt, char *home_dir) {
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
  char *tcompressed_cwd = replace(cwd, home_dir, "~");
  snprintf(*prompt, PROMPT_MAX, "%s[%s@%s] %s%s%s ", RED, username, hostname,
           YEL, tcompressed_cwd, WHT);
  free(tcompressed_cwd);
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
  current_ctx->is_background_process = check_if_background(current_ctx->input);
  if (!determine_out_stream(current_ctx)) {
    printf("clowniSH: Failed to determine output stream.\n");
  }
  char *tilde_expanded = replace(current_ctx->input, "~",current_ctx->home_dir);
  if (!tilde_expanded) {
    return 0;
  }
  if (!parse_envs(tilde_expanded, &current_ctx->parsed_str)) {
    return 0;
  }
  free(tilde_expanded);
  current_ctx->args_count = 0;
  current_ctx->args = tokenize_input(current_ctx->parsed_str, &current_ctx->args_count);
  if (!current_ctx->args[0]) {
    return 1;
  }
  return 0;
}
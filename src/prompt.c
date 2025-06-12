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

int prompt_loop(char ***args, char **input, int *args_count,
                char **parsed_str_buffer, int *is_background_process,
                struct stream_info *current_stream_info, char *home_dir) {
  char *prompt = malloc(PROMPT_MAX);
  if (!prompt) {
    printf("clowniSH: Failed to allocate memory for prompt.\n");
    return 1;
  }
  if (construct_prompt(&prompt, home_dir) == 1) {
    printf("clowniSH: Failed to construct prompt, using default.\n");
    strcpy(prompt, "clowniSH$ ");
  }
  *input = readline(prompt);
  free(prompt);
  if (!input || *input[0] == '\0') {
    return 0;
  }
  add_history(*input);
  *is_background_process = check_if_background(*input);
  if (!determine_stream(*input, current_stream_info)) {
    printf("clowniSH: Failed to determine output stream.\n");
  }
  char *tilde_expanded = replace(*input, "~",home_dir);
  if (!tilde_expanded) {
    return 0;
  }
  if (!parse_envs(tilde_expanded, parsed_str_buffer)) {
    return 0;
  }
  free(tilde_expanded);
  *args_count = 0;
  *args = tokenize_input(*parsed_str_buffer, args_count);
  if (!*args[0]) {
    return 1;
  }
  return 0;
}
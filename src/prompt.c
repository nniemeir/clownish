#include "prompt.h"
#include "parse.h"
#include "error.h"

int construct_prompt(char **prompt, char *home_dir, char *user) {
  char hostname[_SC_HOST_NAME_MAX];
  if (gethostname(hostname, _SC_HOST_NAME_MAX) == -1) {
    perror(program_name);
    return 1;
  }

  char *cwd = malloc(PATH_MAX);
  if (!getcwd(cwd, PATH_MAX)) {
    perror(program_name);
    return 1;
  }

  replace(&cwd, home_dir, "~");

  snprintf(*prompt, PROMPT_MAX, "%s[%s@%s] %s%s%s ", RED, user, hostname, YEL,
           cwd, WHT);

  free(cwd);
  return 0;
}

int prompt_loop(struct repl_ctx *current_ctx) {
  char *prompt = malloc(PROMPT_MAX);
  if (!prompt) {
    fprintf(stderr, malloc_fail_msg, "prompt");
    return 1;
  }

  if (construct_prompt(&prompt, current_ctx->home_dir, current_ctx->user) ==
      1) {
    strcpy(prompt, "clowniSH$ ");
  }

  current_ctx->input = readline(prompt);
  free(prompt);
  if (!current_ctx->input || current_ctx->input[0] == '\0') {
    return 0;
  }

  add_history(current_ctx->input);

  current_ctx->args_count = 0;

  current_ctx->command =
      tokenize_input(current_ctx->input, &current_ctx->args_count);
  if (!current_ctx->command[0]) {
    return 1;
  }

  determine_if_background(current_ctx);
  determine_in_stream(current_ctx);
  determine_out_stream(current_ctx);

  for (unsigned int i = 0; i < current_ctx->args_count; i++) {
    replace(&current_ctx->command[i], "~", current_ctx->home_dir);
    parse_envs(&current_ctx->command[i], current_ctx->user_envs, current_ctx->user_envs_count);
  }
  return 0;
}
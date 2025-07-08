#include "input.h"
#include "config.h"
#include "context.h"
#include "error.h"
#include "parse.h"

int take_input(struct repl_ctx *current_ctx) {
  char *prompt = construct_prompt(current_ctx->home_dir, current_ctx->user);
  if (!prompt) {
    return 1;
  }

  current_ctx->input = readline(prompt);

  free(prompt);

  if (!current_ctx->input || current_ctx->input[0] == '\0') {
    return 0;
  }

  add_history(current_ctx->input);

  return 0;
}

int process_input(struct repl_ctx *current_ctx) {
  current_ctx->commands_count = 0;

  current_ctx->unparsed_commands =
      split_on_pipes(current_ctx->input, &current_ctx->commands_count);

  if (init_repl_vars(current_ctx) == 1) {
    return 1;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->commands[i] = lex_input(current_ctx->unparsed_commands[i],
                                         &current_ctx->args_count[i]);
    if (!current_ctx->commands[i] || !current_ctx->commands[i][0]) {
      return 1;
    }

    determine_if_background(current_ctx, i);

    determine_in_stream(current_ctx, i);

    determine_out_stream(current_ctx, i);

    for (unsigned int j = 0; j < current_ctx->args_count[i]; j++) {
      replace(&current_ctx->commands[i][j], "~", current_ctx->home_dir);
      parse_envs(&current_ctx->commands[i][j], current_ctx->user_envs,
                 current_ctx->user_envs_count);
    }
  }

  return 0;
}

char *construct_prompt(char *home_dir, char *user) {
  static const char *fallback_prompt = "clowniSH$ ";

  char *prompt = malloc(PROMPT_MAX);
  if (!prompt) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  if (debug_mode) {
    snprintf(prompt, PROMPT_MAX, "%s", fallback_prompt);
    return prompt;
  }

  char hostname[_SC_HOST_NAME_MAX];

  if (gethostname(hostname, _SC_HOST_NAME_MAX) == -1) {
    error_msg("Failed to get hostname", true);
    snprintf(prompt, PROMPT_MAX, "%s", fallback_prompt);
    return prompt;
  }

  char *cwd = malloc(PATH_MAX);

  if (!getcwd(cwd, PATH_MAX)) {
    error_msg("Failed to get current working directory", true);
    free(cwd);
    snprintf(prompt, PROMPT_MAX, "%s", fallback_prompt);
    return prompt;
  }

  replace(&cwd, home_dir, "~");

  snprintf(prompt, PROMPT_MAX, "%s[%s@%s] %s%s%s ", RED, user, hostname, YELLOW,
           cwd, CYAN);

  free(cwd);

  return prompt;
}

int init_repl_vars(struct repl_ctx *current_ctx) {
  current_ctx->commands = malloc(current_ctx->commands_count * sizeof(char **));
  if (!current_ctx->commands) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  current_ctx->args_count =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->args_count) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->args_count[i] = 0;
  }

  current_ctx->in_stream_name =
      malloc(current_ctx->commands_count * sizeof(char *));
  if (!current_ctx->in_stream_name) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  current_ctx->in_stream_type =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->in_stream_type) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  current_ctx->out_stream_name =
      malloc(current_ctx->commands_count * sizeof(char *));
  if (!current_ctx->out_stream_name) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  current_ctx->out_stream_type =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->out_stream_type) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->in_stream_name[i] = NULL;
    current_ctx->out_stream_name[i] = NULL;
  }

  return 0;
}

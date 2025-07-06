#include "prompt.h"
#include "error.h"
#include "parse.h"

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

  current_ctx->commands_count = 0;
  current_ctx->unparsed_commands =
      split_on_pipes(current_ctx->input, &current_ctx->commands_count);

  current_ctx->commands = malloc(current_ctx->commands_count * sizeof(char **));
  current_ctx->args_count =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->args_count[i] = 0;
  }

  current_ctx->in_stream_name =
      malloc(current_ctx->commands_count * sizeof(char *));
  if (!current_ctx->in_stream_name) {
    fprintf(stderr, malloc_fail_msg, "in_stream_name");
    return 1;
  }
  current_ctx->in_stream_type =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->in_stream_type) {
    fprintf(stderr, malloc_fail_msg, "in_stream_type");
    return 1;
  }
  current_ctx->out_stream_name =
      malloc(current_ctx->commands_count * sizeof(char *));
  if (!current_ctx->out_stream_name) {
    fprintf(stderr, malloc_fail_msg, "out_stream_name");
    return 1;
  }
  current_ctx->out_stream_type =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->out_stream_type) {
    fprintf(stderr, malloc_fail_msg, "out_stream_type");
    return 1;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->in_stream_name[i] = NULL;
    current_ctx->out_stream_name[i] = NULL;
  }

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->commands[i] = tokenize_input(current_ctx->unparsed_commands[i],
                                              &current_ctx->args_count[i]);
    if (!current_ctx->commands[i][0]) {
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
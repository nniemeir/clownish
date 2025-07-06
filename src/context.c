#include "context.h"
#include "config.h"
#include "envs.h"

void cleanup_ctx(struct repl_ctx *current_ctx) {
  // Allocation of args only occurs if the string is not empty
  if (current_ctx->input[0] != '\0') {
    for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
      if (current_ctx->commands[i]) {
        for (unsigned int j = 0; j < current_ctx->args_count[i]; j++) {
          free(current_ctx->commands[i][j]);
        }
        free(current_ctx->commands[i]);
      }
      if (current_ctx->unparsed_commands[i]) {
        free(current_ctx->unparsed_commands[i]);
      }
      if (current_ctx->in_stream_name[i]) {
        free(current_ctx->in_stream_name[i]);
      }
      if (current_ctx->out_stream_name[i]) {
        free(current_ctx->out_stream_name[i]);
      }
    }
    if (current_ctx->out_stream_name) {
      free(current_ctx->out_stream_name);
    }
    if (current_ctx->out_stream_type) {
      free(current_ctx->out_stream_type);
    }
    if (current_ctx->args_count) {
      free(current_ctx->args_count);
    }
    if (current_ctx->commands) {
      free(current_ctx->commands);
    }
    if (current_ctx->in_stream_name) {
      free(current_ctx->in_stream_name);
    }
    if (current_ctx->in_stream_type) {
      free(current_ctx->in_stream_type);
    }
    if (current_ctx->unparsed_commands) {
      free(current_ctx->unparsed_commands);
    }
  }
  if (current_ctx->input) {
    free(current_ctx->input);
  }
}

void init_current_ctx(struct repl_ctx *current_ctx) {
  current_ctx->home_dir = init_home_dir();
  if (!current_ctx->home_dir) {
    exit(EXIT_FAILURE);
  }

  load_config(current_ctx);

  current_ctx->user = getenv_checked("USER", "Keith");
}
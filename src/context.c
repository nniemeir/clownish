/**
 * context.c
 *
 * Shell context initialization and cleanup.
 *
 * OVERVIEW:
 * The context is the data structure that holds all shell state between REPL
 * iterations, including:
 * - User information (home directory, username)
 * - Current input and parsed commands
 * - I/O redirection settings
 * - User-defined environment variables
 */

#include <stdlib.h>

#include "config.h"
#include "envs.h"

/**
 * clenaup_ctx - Free all dynamically allocated memory in context
 * @current_ctx: Shell context
 *
 * Frees memory allocated during command parsing and execution, called after
 * every command to prevent memory leaks.
 *
 * We check that each variable is non-null before freeing because some fields
 * might not be allocated.
 */
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

    if (current_ctx->unparsed_commands) {
      free(current_ctx->unparsed_commands);
    }
  }

  if (current_ctx->input) {
    free(current_ctx->input);
  }
}

/**
 * init_current_ctx - Initialize shell context at startup
 * @current_ctx: context to initialize
 *
 * Sets up persistent shell state that lasts for the entire session.
 *
 * If config loading fails, shell continues as user config is optional.
 */
void init_current_ctx(struct repl_ctx *current_ctx) {
  current_ctx->home_dir = init_home_dir();
  if (!current_ctx->home_dir) {
    exit(EXIT_FAILURE);
  }

  load_config(current_ctx);

  /* 
   * Default to Keith if we can't get the value of USER. You know who you are
   * Keith, you know what you did. -_- 
   */
  current_ctx->user = getenv_checked("USER", "Keith");
}

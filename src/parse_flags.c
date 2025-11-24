#include <string.h>

#include "error.h"
#include "parse.h"

void determine_if_background(struct repl_ctx *current_ctx,
                             unsigned int command_index) {
  current_ctx->is_background_process = 0;

  if (strcmp(current_ctx->commands[command_index]
                                  [current_ctx->args_count[command_index] - 1],
             "&") == 0) {
    if (command_index - 1 != current_ctx->commands_count) {
      error_msg("Background operator can only be specified for the last "
                "command in the pipeline.",
                false);
      return;
    }

    current_ctx->is_background_process = 1;

    remove_arg(current_ctx->commands[command_index],
               &current_ctx->args_count[command_index],
               current_ctx->args_count[command_index] - 1);
  }
}

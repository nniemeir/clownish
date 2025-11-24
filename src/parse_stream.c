#include <fcntl.h>
#include <string.h>

#include "error.h"
#include "parse.h"

void determine_in_stream(struct repl_ctx *current_ctx,
                         unsigned int command_index) {
  for (unsigned int i = 0; i < current_ctx->args_count[command_index]; i++) {
    if (strcmp(current_ctx->commands[command_index][i], "<") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }

      current_ctx->in_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->in_stream_name[command_index]) {
        error_msg(strdup_fail_msg, true);
        return;
      }

      // Called twice to remove lt and the stream name
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
    }
  }
}

void determine_out_stream(struct repl_ctx *current_ctx,
                          unsigned int command_index) {
  for (unsigned int i = 0; i < current_ctx->args_count[command_index]; i++) {
    if (strcmp(current_ctx->commands[command_index][i], ">") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }

      current_ctx->out_stream_type[command_index] = O_WRONLY;

      current_ctx->out_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->out_stream_name[command_index]) {
        error_msg(strdup_fail_msg, true);
        return;
      }

      // Called twice to remove gt and the stream name
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);

      return;
    }

    if (strcmp(current_ctx->commands[command_index][i], ">>") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }

      current_ctx->out_stream_type[command_index] = O_APPEND;

      current_ctx->out_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->out_stream_name[command_index]) {
        error_msg(strdup_fail_msg, true);
        return;
      }

      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);

      return;
    }
  }
}

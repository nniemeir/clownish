/**
 * parse_flags.c
 *
 * Parser for background process operator (&).
 *
 * OVERVIEW:
 * Responsible for detecting and parsing the background operator. POSIX
 * specification states that when a command ends with &, it should run
 * asynchronously without blocking the shell. Only the LAST command in a
 * pipeline can be backgrounded because all commands in a pipeline must run
 * together.
 */

#include <string.h>

#include "error.h"
#include "parse.h"

/**
 * determine_if_background - Check for background operator
 * @current_ctx: Shell context with parsed commands
 * @command_index: Which command in pipeline to check
 *
 * Looks for '&' as the last argument of the command. If found and the command
 * is the last in the pipeline, removes & from arguments since it is not a
 * program argument and sets is_background_process to true.
 *
 * When exec.c sees that is_background_process is true, it calls setpgid(0, 0)
 * to create a new process group. This prevents SIGINT (Ctrl+C) from killing the
 * background job.
 */
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

    /* Remove '&' from arguments */
    remove_arg(current_ctx->commands[command_index],
               &current_ctx->args_count[command_index],
               current_ctx->args_count[command_index] - 1);
  }
}

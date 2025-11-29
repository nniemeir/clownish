/**
 * parse_stream.c
 *
 * Parser for I/O redirection operators.
 *
 * OVERVIEW:
 * This file handles detecting and parsing the shell's I/O redirection:
 * Input redirection: < filename
 * Output redirection: > filename
 * Append redirection: >> filename
 *
 * It is important that we remove the operators and their arguments as the
 * programs themselves don't understand shell syntax.
 */

#include <fcntl.h>
#include <string.h>

#include "error.h"
#include "parse.h"

/**
 * determine_in_stream - Parse input redirection operator
 * @current_ctx: Shell context
 * @command_index: Which command in the pipeline to check
 *
 * Searches for the '<' operator in the command's arguments. If found:
 * - Extracts the filename that follows it
 * - Stores filename in in_stream_name[command_index]
 * - Removes '<' and filename from arguments
 */
void determine_in_stream(struct repl_ctx *current_ctx,
                         unsigned int command_index) {
  for (unsigned int i = 0; i < current_ctx->args_count[command_index]; i++) {
    if (strcmp(current_ctx->commands[command_index][i], "<") == 0) {
      /* Check if there's a filename after '<' */
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

      /* Remove '<' and stream name */
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
    }
  }
}

/**
 * determine_out_stream - Parse input redirection operator
 * @current_ctx: Shell context
 * @command_index: Which command in the pipeline to check
 *
 * Searches for the '>' and ">>" operators in the command's arguments. If found:
 * - Extracts the filename that follows it
 * - Stores filename in in_stream_name[command_index]
 * - Removes operator and filename from arguments
 */
void determine_out_stream(struct repl_ctx *current_ctx,
                          unsigned int command_index) {
  for (unsigned int i = 0; i < current_ctx->args_count[command_index]; i++) {
    /* Check for '>': write mode */
    if (strcmp(current_ctx->commands[command_index][i], ">") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }

      /* write mode (O_WRONLY) overwrites the files contents */
      current_ctx->out_stream_type[command_index] = O_WRONLY;

      current_ctx->out_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->out_stream_name[command_index]) {
        error_msg(strdup_fail_msg, true);
        return;
      }

      /* Remove ">" and filename from arguments */
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);

      return;
    }

    /* Check for '>>': append mode*/
    if (strcmp(current_ctx->commands[command_index][i], ">>") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }

      /* 
       * O_APPEND mode positions writes at the end of the file, preserving
       * existing content.
       */
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

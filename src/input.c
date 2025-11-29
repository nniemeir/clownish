/**
 * input.c
 *
 * User input handling and prompt generation.
 *
 * OVERVIEW:
 * Responsible for the reading part of the Read-Eval-Print Loop. This project is
 * primarily focused on the systems programming aspects of POSIX-like shells, so
 * line editing functionality is deferred to GNU Readline.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "config.h"
#include "error.h"
#include "input.h"
#include "parse.h"

/**
 * take_input - Display prompt and read user input
 * @current_ctx: Shell context
 *
 * Uses GNU readline to get input from the user. Readline provides line editing
 * and command history functionality.
 *
 * Return: 0 on success, -1 on error
 */
int take_input(struct repl_ctx *current_ctx) {
  char *prompt = construct_prompt(current_ctx->home_dir, current_ctx->user);
  if (!prompt) {
    return -1;
  }

  /* Display the prompt and take user input. */
  current_ctx->input = readline(prompt);

  free(prompt);

  /* We don't bother trying to add empty input to history */
  if (!current_ctx->input || current_ctx->input[0] == '\0') {
    return 0;
  }

  add_history(current_ctx->input);

  return 0;
}

/**
 * process_input - Parse input into executable commands
 * @current_ctx: Shell context
 *
 * This does quite a bit:
 * - Splits input on pipes to get individual commands
 * - Initialize arrays for command data
 * - Tokenize each command into arguments
 * - Parse special operators (&, <, >, >>)
 * - Expand environment variables and tilde (~)
 *
 * Return: 0 on success, -1 on error
 */
int process_input(struct repl_ctx *current_ctx) {
  current_ctx->commands_count = 0;

  /* Split input on ' | ' to get individual commands */
  current_ctx->unparsed_commands =
      split_on_pipes(current_ctx->input, &current_ctx->commands_count);

  /**
   * Allocate arrays fro command metadata now that we know how many commands we
   * have in the pipeline
   */
  if (init_repl_vars(current_ctx) == -1) {
    return -1;
  }

  /* Process each command in the pipeline */
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    /* Tokenize command into arguments */
    current_ctx->commands[i] = lex_input(current_ctx->unparsed_commands[i],
                                         &current_ctx->args_count[i]);
    if (!current_ctx->commands[i] || !current_ctx->commands[i][0]) {
      return -1;
    }

    /* Parse special operators and remove them from arguments */
    determine_if_background(current_ctx, i);

    determine_in_stream(current_ctx, i);

    determine_out_stream(current_ctx, i);

    /* Expand environment variables and tilde (~) */
    for (unsigned int j = 0; j < current_ctx->args_count[i]; j++) {
      replace(&current_ctx->commands[i][j], "~", current_ctx->home_dir);
      parse_envs(&current_ctx->commands[i][j], current_ctx->user_envs,
                 current_ctx->user_envs_count);
    }
  }

  return 0;
}

/**
 * construct_prompt - Build shell prompt string
 * @home_dir: User's home directory
 * @user: Username to display
 *
 * Creates a color-coded prompt showing username, hostname, and current working
 * directory.
 *
 * Return: Allocated prompt string, NULL on error
 */
char *construct_prompt(char *home_dir, char *user) {
  /*
   * We use the fallback prompt if in debug mode or unable to get information
   * needed to generate the normal prompt.
   */
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

  /*
   * We get the system's network hostname, this is usually stored in
   * /etc/hostname.
   */
  if (gethostname(hostname, _SC_HOST_NAME_MAX) == -1) {
    error_msg("Failed to get hostname", true);
    snprintf(prompt, PROMPT_MAX, "%s", fallback_prompt);
    return prompt;
  }

  char *cwd = malloc(PATH_MAX);

  /*
   * We get the current working directory, this could also be done by using the
   * readlink syscall on the /proc/self/cwd symlink (Linux-only).
   */
  if (!getcwd(cwd, PATH_MAX)) {
    error_msg("Failed to get current working directory", true);
    free(cwd);
    snprintf(prompt, PROMPT_MAX, "%s", fallback_prompt);
    return prompt;
  }

  /* Condense the home_dir to tilde in the prompt for brevity */
  replace(&cwd, home_dir, "~");

  /* Construct the prompt now that we have all of the necessary information */
  snprintf(prompt, PROMPT_MAX, "%s[%s@%s] %s%s%s ", RED, user, hostname, YELLOW,
           cwd, CYAN);

  free(cwd);

  return prompt;
}

/**
 * init_repl_vars - Allocate arrays for command metadata
 * @current_ctx: Shell context
 *
 * Allocates arrays for storing command metadata, including:
 * - commands[i]: argument array for command i
 * - args_count[i]: how many args in commands[i]
 * - in_stream_name[i]: input file for command i (or NULL)
 * - out_stream_name[i]: output file for command i (or NULL)
 * - out_stream_type[i]: O_WRONLY or O_APPEND
 *
 * All arrays are sized by commands_count, which was set by split_on_pipes, so
 * that we only allocate as much memory as we need.
 *
 * Return: 0 on success, -1 on error
 */
int init_repl_vars(struct repl_ctx *current_ctx) {
  /* Array of command arrays */
  current_ctx->commands = malloc(current_ctx->commands_count * sizeof(char **));
  if (!current_ctx->commands) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  /* Array of argument counts */
  current_ctx->args_count =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->args_count) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  /* Initialize all arg counts to 0 */
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->args_count[i] = 0;
  }

  /* Array of input file names */
  current_ctx->in_stream_name =
      malloc(current_ctx->commands_count * sizeof(char *));
  if (!current_ctx->in_stream_name) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  /* Array of output file names */
  current_ctx->out_stream_name =
      malloc(current_ctx->commands_count * sizeof(char *));
  if (!current_ctx->out_stream_name) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  /* Array of output file types (O_WRONLY or O_APPEND) */
  current_ctx->out_stream_type =
      malloc(current_ctx->commands_count * sizeof(unsigned int));
  if (!current_ctx->out_stream_type) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  /* Initialize stream names to NULL to indicate no redirection */
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    current_ctx->in_stream_name[i] = NULL;
    current_ctx->out_stream_name[i] = NULL;
  }

  return 0;
}

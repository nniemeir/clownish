/**
 * main.c
 *
 * Entry point for ClowniSH.
 *
 * OVERVIEW:
 * Responsible for CLI argument parsing, initializing the shell environment, and
 * the REPL (Read-Eval-Print Loop) that blocks until the user enters the "exit"
 * command or SIGINT is received.
 *
 * KEY CONCEPTS:
 * - REPL: This is the pattern that shells use. Read input, evaluate, print
 * results, and loop.
 * - Context Management: We keep the shell's state in a repl_ctx struct instance
 * that we pass around to avoid having a ton of global variables
 * - GNU Readline: We use this library to provide command history and input,
 *   this keeps the project's focus on the OS level functionality
 *
 * SECURITY:
 * The root user cannot run this shell, as it is designed to be unpredictable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "error.h"
#include "exec.h"
#include "history.h"
#include "input.h"
#include "signals.h"
#include "tease.h"

/**
 * process_args - Parse CLI options
 * @argc: Argument count from main
 * @argv: Argument array from main
 *
 * FLAGS:
 * -d: Debug mode (disables color overrides and dynamically generated prompt)
 * -h: Display program usage and exit
 * -p: Polite mode (disables all teasing functionality)
 * -v: Display version and exit

 * Uses getopt() to handle startup options, this is the standard POSIX method to
 * do so.
 */
void process_args(int argc, char *argv[]) {
  int c;
  while ((c = getopt(argc, argv, "dhpv")) != -1) {
    switch (c) {
    case 'd':
      debug_mode = true;
      break;
    case 'h':
      printf("Usage: clownish [options]\n");
      printf("Options:\n");
      printf("  -d               enable debug mode\n");
      printf("  -h               Show this help message\n");
      printf("  -p               Enable polite mode\n");
      printf("  -v               Show version info\n");
      exit(EXIT_SUCCESS);
    case 'p':
      teasing_enabled = false;
      break;
    case 'v':
      printf("clowniSH 0.231969420: Malevolent Marlin\n");
      exit(EXIT_SUCCESS);
    case '?':
      fprintf(stderr, "Unknown option '-%c'. Run with -h for options.\n",
              optopt);
      exit(EXIT_FAILURE);
    }
  }
}

/**
 * skip_execution - Check if command should be blocked
 * @current_ctx: Shell context containing parsed commands
 *
 * Sometimes ClowniSH needs to protect the user from themselves and refuse to
 * run unscrupulous software, do not resist.
 *
 * Return: true if command blacklisted, false otherwise
 */
bool skip_execution(struct repl_ctx *current_ctx) {
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    if (program_is_blacklisted(current_ctx->commands[i][0])) {
      return true;
    }
  }
  return false;
}

/**
 * repl - Read-Eval-Print Loop
 * @current_ctx: Shell context
 * @hist_file: Path to history file for saving on exit
 *
 * This is the core of the shell, it loops until the user enters the "exit"
 * command or presses Ctrl+D.
 * - Reads user input
 * - Parses it into commands and arguments
 * - Executes if not blacklisted
 * - Randomly teases the user about their software choices
 * - Cleans up allocated memory
 */
void repl(struct repl_ctx *current_ctx, char *hist_file) {
  while (current_ctx->receiving) {
    if (take_input(current_ctx) == -1) {
      cleanup_ctx(current_ctx);
      close_history(hist_file);
      exit(EXIT_FAILURE);
    }

    /* Empty input does not need processing */
    if (current_ctx->input[0] == '\0') {
      cleanup_ctx(current_ctx);
      continue;
    }

    if (process_input(current_ctx) == -1) {
      cleanup_ctx(current_ctx);
      close_history(hist_file);
      exit(EXIT_FAILURE);
    }

    if (skip_execution(current_ctx)) {
      cleanup_ctx(current_ctx);
      continue;
    }

    if (exec(current_ctx) == -1) {
      cleanup_ctx(current_ctx);
      continue;
    }

    handle_teasing(current_ctx);

    cleanup_ctx(current_ctx);
  }
}

/**
 * main - Shell entry point
 * @argc: Argument count
 * @argv: Argument array
 *
 * Orchestrates initialization and cleanup on exit.
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
  /**
   * The UID is checked to ensure that UID 0 (Root) is not the one running
   * ClowniSH, we have to draw the line somewhere.
   */
  if (geteuid() == 0) {
    error_msg("You attempted to run this shell as root, I cannot sanction your "
              "buffoonery.\n",
              false);
    exit(EXIT_FAILURE);
  }

  process_args(argc, argv);

  struct repl_ctx current_ctx;

  init_current_ctx(&current_ctx);

  char *hist_file = init_history(current_ctx.home_dir);

  if (!hist_file) {
    exit(EXIT_FAILURE);
  }

  if (init_sig_handler() == -1) {
    exit(EXIT_FAILURE);
  }

  /*
   * Seed the random number generator with the current time to ensure variety
   * between each run. RNG is used in this shell to decide when to tease the
   * user about their software choices.
   */
  srand(time(NULL));

  current_ctx.receiving = 1;
  current_ctx.input = NULL;

  repl(&current_ctx, hist_file);

  close_history(hist_file);

  exit(EXIT_SUCCESS);
}

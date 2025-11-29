/**
 * tease_logic.c
 *
 * Main teasing system coordinator.
 *
 * OVERVIEW:
 * Responsible for orchestrating the shell's teasing features. A few steps have
 * been taken to avoid these messages being overbearing:
 * - Teasing only occurs if -p (polite flag) not given at startup
 * - Only teases every other command
 * - Each category of teasing has a 20% chance of occuring per command
 */

#include <stdlib.h>

#include "tease.h"

bool teasing_enabled = true;

/**
 * tease_roll: Randomly select and execute a teasing action
 * @current_ctx: Shell context
 * @command_index: Which command in pipeline to potentially tease about
 *
 * Rolls a random number and selects a teasing category based on ranges, with
 * each category having a 20% chance of being selected.
 */
void tease_roll(struct repl_ctx *current_ctx, unsigned int command_index) {
  const int random_num = rand() % 101;

  if (random_num <= 20) {
    tease_distro();
    return;
  }

  if (random_num <= 40) {
    /*
     * Search the sorted known_programs array for the command. If it is found,
     * the corresponding message is printed. Otherwise, does nothing.
     */
    joke_binary_search(known_programs, NUM_OF_KNOWN_PROGRAMS,
                       current_ctx->commands[command_index][0]);
    return;
  }

  if (random_num <= 60) {
    tease_kernel();
    return;
  }

  if (random_num <= 80) {
    tease_terminal();
    return;
  }

  tease_desktop();
}

/**
 * handle_teasing - Entry point for teasing system
 * @current_ctx: Shell context
 *
 * Decides whether to tease the user after command execution, uses a static
 * toggle to alternate between teasing and not teasing. In a pipeline with
 * multiple commands, we loop through each command but only tease about
 * alternating ones based on the toggle.
 */
void handle_teasing(struct repl_ctx *current_ctx) {
  static bool teasing_current_command = true;

  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    if (teasing_enabled && current_ctx->commands[i][0][0] != '\0') {
      if (teasing_current_command) {
        tease_roll(current_ctx, i);
      }
      teasing_current_command = !teasing_current_command;
    }
  }
}

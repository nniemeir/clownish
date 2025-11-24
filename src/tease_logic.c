#include <stdlib.h>

#include "tease.h"

bool teasing_enabled = true;

void tease_roll(struct repl_ctx *current_ctx, unsigned int command_index) {
  const int random_num = rand() % 101;

  if (random_num <= 20) {
    tease_distro();
    return;
  }

  if (random_num <= 40) {
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

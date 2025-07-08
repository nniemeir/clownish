#include "builtins.h"
#include "error.h"
#include "tease.h"

int cat(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    return 0;
  }

  const int random_num = rand() % 101;

  if (random_num <= 10) {
    printf(" /\\_/\\\n");
    printf("( o.o )\n");
    printf(" > ^ <\n");
    printf("You requested my presence %s?\n", current_ctx->user);
    return 1;
  }

  return 0;
}

int cd(struct repl_ctx *current_ctx) {
  if (!current_ctx->commands[0][1]) {
    current_ctx->commands[0][1] = current_ctx->home_dir;
  }

  if (chdir(current_ctx->commands[0][1]) == -1) {
    error_msg("Failed to change working directory", true);
    fprintf(stderr, blame_user_msg, current_ctx->user);
    return -1;
  }

  return 1;
}

int cler(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    return 0;
  }

  fprintf(stderr,
          "Perhaps you meant to type clear but "
          "made a typo in your haste, let's take a breather for a moment.\n");
  sleep(10);
  printf("Don't you feel better %s?\n", current_ctx->user);

  return 1;
}

int exit_builtin(struct repl_ctx *current_ctx) {
  current_ctx->receiving = 0;
  printf("Finally giving up, %s?\n", current_ctx->user);
  return 1;
}

int help(struct repl_ctx *current_ctx) {
  if (!teasing_enabled) {
    printf("cd - change directory\n");
    printf("exit - exit shell\n");
    printf("help - display this message\n");
    return 1;
  }

  printf("You aren't seriously asking me to hold your hand, are you %s?\n",
         current_ctx->user);

  return 1;
}

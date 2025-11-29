/**
 * builtins.c
 * Built-in shell commands implementation.
 *
 * OVERVIEW:
 * Built-in commands are executed directly in the shell without forking a new
 * process. This is only needed for commands that modify the shell's state.
 *
 * TEASING:
 * Some of these built-ins are only used to randomly override expected output of
 * common commands. An example being cat, which prints an ASCII cat picture 10%
 * of the time that the user tries to run cat to concatenate files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"
#include "error.h"
#include "tease.h"

/**
 * cat - Joke version of cat command
 * @current_ctx: Shell context (for user name)
 *
 * 10% of the time, an ASCII cat will be printed when users try to run the cat
 * command from GNU Coreutils.
 *
 * Return: 0 if normal cat should run instead, 1 otherwise
 */
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

/**
 * cd - Change directory builtin
 * @current_ctx: Shell context with command arguments
 *
 * Changes the shell's working directory using chdir(). If run with no argument,
 * changes directory to user's home.
 *
 * cd has to be a builtin because forking a child process and running chdir()
 * there would have no effect on the parent.
 *
 * Return: 1 on success, -1 on error
 */
int cd(struct repl_ctx *current_ctx) {
  if (!current_ctx->commands[0][1]) {
    current_ctx->commands[0][1] = current_ctx->home_dir;
  }

  /* Blame the user by name on error, because it is obviously their fault */
  if (chdir(current_ctx->commands[0][1]) == -1) {
    error_msg("Failed to change working directory", true);
    fprintf(stderr, blame_user_msg, current_ctx->user);
    return -1;
  }

  return 1;
}

/**
 * cler - Typo handler for "clear" command
 * @current_ctx: Shell context (for user name)
 *
 * This catches the common typo for the "clear" command
 *
 * Return: 0 if not handling typo, 1 otherwise
 */
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

/**
 * exit_builtin - Exit the shell
 * @current_ctx: Shell context
 *
 * Stops the REPL loop by setting receiving to 0. If we simply called exit(), we
 * would be skipping cleanup.
 *
 * Return: 1 always
 */
int exit_builtin(struct repl_ctx *current_ctx) {
  current_ctx->receiving = 0;
  printf("Finally giving up, %s?\n", current_ctx->user);
  return 1;
}

/**
 * help - Display builtins (maybe)
 * 
 * Return: 1 always
 */
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

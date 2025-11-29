/**
 * builtins.h
 *
 * Declares the interface for commands that are implemented directly in the
 * shell rather than as external programs. These are necessary for commands that
 * need to modify the shell's state.
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "context.h"

/**
 * cat - Joke version of cat command
 * @current_ctx: Shell context (for user name)
 *
 * 10% of the time, an ASCII cat will be printed when users try to run the cat
 * command from GNU Coreutils.
 *
 * Return: 0 if normal cat should run instead, 1 otherwise
 */
int cat(struct repl_ctx *current_ctx);

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
int cd(struct repl_ctx *current_ctx);

/**
 * cler - Typo handler for "clear" command
 * @current_ctx: Shell context (for user name)
 *
 * This catches the common typo for the "clear" command
 *
 * Return: 0 if not handling typo, 1 otherwise
 */
int cler(struct repl_ctx *current_ctx);

/**
 * exit_builtin - Exit the shell
 * @current_ctx: Shell context
 *
 * Stops the REPL loop by setting receiving to 0. If we simply called exit(), we
 * would be skipping cleanup.
 *
 * Return: 1 always
 */
int exit_builtin(struct repl_ctx *current_ctx);

/**
 * help - Display builtins (maybe)
 *
 * Return: 1 always
 */
int help(struct repl_ctx *current_ctx);

#endif

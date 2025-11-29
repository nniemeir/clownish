/** 
 * exec.h
 *
 * Declares the main exec function and related structures for running commands.
 */

#ifndef EXEC_H
#define EXEC_H

#include "context.h"

/**
 * NUM_OF_BUILTINS - Number of built-in commands
 * 
 * Used to size the builtins array and loop through it during execution.
 */
#define NUM_OF_BUILTINS 5

/**
 * command_associations - Builtin command lookup table
 *
 * Maps command names to their implementation functions.
 */
struct command_associations {
  char command_name[255];
  int (*command_function)(struct repl_ctx *);
};

/**
 * exec - Execute command pipeline
 * @current_ctx: Shell context
 *
 * This does quite a bit: 
 * - Checks if first command is a builtin and executes it if so
 * - Create pipes if there are multiple commands
 * - Fork child process for each command
 * - Set up pipes and I/O redirection in child
 * - Close all pipe file descriptors in parent and child
 * - Execute program in child with execvp
 * - Wait for all children to finish in parent (unless background process)
 * 
 * Return: 0 on success, -1 on error
 */
int exec(struct repl_ctx *current_ctx);

#endif

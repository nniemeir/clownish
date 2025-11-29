/**
 * input.h
 *
 * User input handling and prompt generation.
 */

#ifndef INPUT_H
#define INPUT_H

#include "context.h"

/**
 * ANSI Color codes for terminal output.
 *
 * These escape sequences change terminal text color. They work in most modern
 * terminals but not all, which is why debug mode disables colors.
 */
#define CYAN "\033[36m"
#define RED "\x1b[31m"
#define WHITE "\x1b[37m"
#define YELLOW "\x1b[33m"

/**
 * take_input - Display prompt and read user input
 * @current_ctx: Shell context
 *
 * Uses GNU readline to get input from the user. Readline provides line editing
 * and command history functionality.
 *
 * Return: 0 on success, -1 on error
 */
int take_input(struct repl_ctx *current_ctx);

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
int process_input(struct repl_ctx *current_ctx);

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
char *construct_prompt(char *home_dir, char *user);

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
int init_repl_vars(struct repl_ctx *current_ctx);

#endif

/**
 * parse.h
 *
 * Declares the parsing pipeline that transforms raw user input into structured
 * command data ready for execution.
 */

#ifndef PARSE_H
#define PARSE_H

#include "context.h"

/**
 * NULL_TERMINATOR_LENGTH - Size of null terminator
 *
 * Used when allocating strings to ensure space for '\0'
 */
#define NULL_TERMINATOR_LENGTH 1

/* COMMANDS_MAX - Maximum number of commands in pipeline */
#define COMMANDS_MAX 64

/**
 * ENV_MAX - Maximum environment variable name length
 *
 * Buffer size for parsing variables names during expansion.
 */
#define ENV_MAX 4096

/**
 * PIPES_MAX - Maximum number of pipes (commands - 1)
 *
 * Each pipe consumes 2 file descriptors, so the number of pipes is limited to
 * prevent resource exhaustion.
 */
#define PIPES_MAX 64

/**
 * PROMPT_MAX - Maximum prompt length
 *
 * Buffer size for shell prompt string.
 */
#define PROMPT_MAX (_SC_HOST_NAME_MAX + _SC_LOGIN_NAME_MAX + PATH_MAX)

/**
 * TOKENS_MAX - Initial token buffer size
 *
 * Starting size for argument array during tokenization.
 * If command has more arguments, buffer is reallocated (grows by TOKENS_MAX)
 */
#define TOKENS_MAX 64

/**
 * remove_arg - Remove argument from command array
 * @command: Array of command arguments
 * @args_count: Pointer to argument count (decremented)
 * @args_index: Index of argument to remove
 *
 * Removes an argument from the command array by shifting all subsequent
 * elements down by one position. This is used to remove shell syntax elements
 * before executing programs.
 */
void remove_arg(char **args, unsigned int *args_count, unsigned int arg_index);

/**
 * replace - Find and replace substring in string
 * @original_str: Pointer to string to modify (modified in-place)
 * @original_substr: Substring to find
 * @new_substr: Substring to replace with
 *
 * Replaces all occurrences of original_substr with new_substr.
 */
void replace(char **original_str, const char *original_substr,
             const char *new_substr);

/**
 * determine_if_background - Check for background operator
 * @current_ctx: Shell context with parsed commands
 * @command_index: Which command in pipeline to check
 *
 * Looks for '&' as the last argument of the command. If found and the command
 * is the last in the pipeline, removes & from arguments since it is not a
 * program argument and sets is_background_process to true.
 *
 * When exec.c sees that is_background_process is true, it calls setpgid(0, 0)
 * to create a new process group. This prevents SIGINT (Ctrl+C) from killing the
 * background job.
 */
void determine_if_background(struct repl_ctx *current_ctx,
                             unsigned int command_index);

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
                         unsigned int command_index);

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
                          unsigned int command_index);

/**
 * parse_envs - Expand environment variables in argument
 * @arg: Pointer to argument string (will be modified)
 * @user_envs: Array of user-defined variables from config
 * @user_envs_count: Number of user variables
 *
 * Searches for "$VARNAME" pattern in the argument and replaces it with the
 * variable's value. The replacement is done in-place by modifying what arg
 * points to.
 */
void parse_envs(char **arg, struct user_env *user_envs,
                unsigned int user_envs_count);

/**
 * split_on_pipes - Split command line into individual commands
 * @line: Full command string
 * @commands_count: Output parameter - number of commands found
 *
 * Returns: Array of command strings, NULL on error
 */
char **split_on_pipes(const char *line, unsigned int *commands_count);

/**
 * lex_input - Tokenize a single command into arguments
 * @line: Command string to tokenize (modified by strtok)
 * @args_count: Output parameter - number of arguments found
 *
 * Uses strtok() to split on whitespace (spaces, tabs, newlines). Each token is
 * duplicated with strdup so it persists after strtok modifies the original
 * string.
 *
 * Return: Array of strings (NULL-terminated), NULL on error
 */
char **lex_input(char *line, unsigned int *args_count);

#endif

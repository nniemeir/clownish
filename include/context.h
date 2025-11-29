/**
 * context.h
 *
 * Defines the repl_ctx struct that holds shell state across REPL iterations.
 */

#ifndef CONTEXT_H
#define CONTEXT_H

/**
 * user_env - User-defined environment variable
 *
 * Stores a single NAME=VALUE pair from config file.
 */
struct user_env {
  char *name;
  char *value;
};

/**
 * repl_ctx - Complete shell state
 *
 * Contains everything the shell needs to know between commands:
 * - User information and directories
 * - Current command being parsed/executed
 * - I/O redirection settings
 * - Custom environment variables
 *
 * FIELDS:
 *
 * PERSISTENT (set once, used throughout):
 * @home_dir: User's home directory from $HOME
 * @config_filename: Path to ~/.clownrc config file
 * @user_envs: Array of user-defined variables
 * @user_envs_count: Number of user variables
 * @user: Username
 * @receiving: Loop control flag (1=running, 0=exit)
 *
 * TEMPORARY (allocated/freed each command):
 * @input: Raw input string from readline
 * @unparsed_commands: Array of command strings split on pipes
 * @commands: 2D array of parsed command arguments
 * @commands_count: Number of commands in pipeline
 * @args_count: Array of argument counts per command
 *
 * I/O REDIRECTION:
 * @in_stream_name: Input file names (< filename)
 * @out_stream_name: Output file names (> or >> filename)
 * @out_stream_type: Output modes (O_WRONLY or O_APPEND)
 *
 * PROCESS CONTROL:
 * @is_background_process: Whether command ends with & (background process)
 */
struct repl_ctx {
  /* Persistent user information */
  char *home_dir;
  char *config_filename;
  struct user_env *user_envs;
  unsigned int user_envs_count;
  char *user;
  int receiving;
  /* Current command data*/
  char *input;
  char ***commands;
  char **unparsed_commands;
  unsigned int commands_count;
  unsigned int *args_count;
  /* I/O Redirection */
  int is_background_process;
  char **in_stream_name;
  char **out_stream_name;
  int *out_stream_type;
};

/**
 * clenaup_ctx - Free temporary command data
 * @current_ctx: Context to clean up
 *
 * Frees all memory allocated during command parsing and execution
 */
void cleanup_ctx(struct repl_ctx *current_ctx);

/**
 * init_current_ctx - Initialize persistent shell state
 * @current_ctx: Context to initialize
 *
 * Sets up data that persists for the entire shell session. Exits shell if home
 * directory can't be resolved (critical failure).
 */
void init_current_ctx(struct repl_ctx *current_ctx);

#endif

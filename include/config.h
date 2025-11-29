/**
 * config.h
 *
 * Functions for loading and accessing user-defined environment variables from
 * ~/.clownrc configuraiton file.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "context.h"

/**
 * debug_mode - Global flag for debug mode
 *
 * When this is true, the fallback prompt is used and ANSI color-coding is
 * disabled.
 */
extern bool debug_mode;

/**
 * get_user_env - Look up user-defined variable
 * @var_name: Variable name to find
 * @user_envs: Array of user variables
 * @user_envs_count: Size of array
 *
 * The configuration file is generally pretty small for most users, so linear
 * search is fine here.
 *
 * Return: Variable value if found, NULL otherwise
 */
char *get_user_env(const char *var_name, struct user_env *user_envs,
                   unsigned int user_envs_count);

/**
 * load_config - Main config file loading function
 * @current_ctx: Shell context
 *
 * Failing to load the config file is non-fatal, the shell can work fine without
 * it.
 *
 * Return: 0 on success, -1 on error
 */
int load_config(struct repl_ctx *current_ctx);

#endif

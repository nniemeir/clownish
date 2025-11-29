/**
 * parse_envs.c
 *
 * Environment variable expansion for command arguments.
 *
 * OVERVIEW:
 * Responsible for expanding environment variables in command arguments using
 * both user-defined variables in ~/.clownrc and system environment variables.
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "error.h"
#include "parse.h"

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
                unsigned int user_envs_count) {
  char *start = *arg;

  /* Search for '$' to find start of variable */
  char *env_position = strstr(start, "$");

  if (!env_position) {
    return;
  }

  /* Check for syntax error: '$' with no variable name afterwards */
  if (strcmp(env_position + 1, "$") == 0) {
    error_msg("Syntax error", false);
    return;
  }

  char var_name[ENV_MAX];
  unsigned int i = 0;

  /* Move past '$' */
  start++;

  while (*start && (isalnum(*start) || *start == '_')) {
    var_name[i++] = *start++;
  }

  /* Ensure null termination */
  var_name[i] = '\0';

  char *env_value;

  /* Check user-defined variables first */
  if (user_envs_count > 0) {
    env_value = get_user_env(var_name, user_envs, user_envs_count);
    if (env_value) {
      *arg = env_value;
      return;
    }
  }

  /* Check system environment variables */
  env_value = getenv(var_name);
  if (!env_value) {
    error_msg(env_fail_msg, false);
    env_value = "";
    return;
  }

  /* TODO: Use replace() here instead, this leaks memory ya dumb beaver */
  *arg = strdup(env_value);
  if (!*arg) {
    error_msg(strdup_fail_msg, true);
  }

  return;
}

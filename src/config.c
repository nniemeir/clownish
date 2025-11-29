/**
 * config.c
 *
 * User configuration file parser.
 *
 * OVERVIEW:
 * Responsible for handling loading and parsing the user's config file
 * (~/.clownrc). Config file defines settings using KEY=VALUE syntax.
 *
 * ERROR HANDLING:
 * Config loading failures are reported but don't stop initialization. The shell
 * can run without a configuration.
 */

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "context.h"
#include "error.h"
#include "file.h"

bool debug_mode = false;

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
                   unsigned int user_envs_count) {
  for (unsigned int i = 0; i < user_envs_count; i++) {
    if (strcmp(var_name, user_envs[i].name) == 0) {
      return user_envs[i].value;
    }
  }
  return NULL;
}

/**
 * construct_config_path - Build path to config file
 * @current_ctx: Shell context
 *
 * Creates the full path ~/.clownrc using previously fetched home_dir.
 *
 * Return: 0 on success, -1 on error
 */
int construct_config_path(struct repl_ctx *current_ctx) {
  current_ctx->config_filename = malloc(PATH_MAX);
  if (!current_ctx->config_filename) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  snprintf(current_ctx->config_filename, PATH_MAX, "%s/.clownrc",
           current_ctx->home_dir);

  return 0;
}

/**
 * count_user_envs - Count variable definitions in config file
 * @current_ctx: Shell context
 * @config_file_contents: File contents as string
 *
 * Counts '=' characters to determine how many variables are defined: this lets
 * us know how much memory to allocate for the user_envs array.
 *
 * Return: 0 if variables found, -1 if empty config
 */
int count_user_envs(struct repl_ctx *current_ctx, char *config_file_contents) {
  current_ctx->user_envs_count = 0;

  for (const char *temp = config_file_contents; (temp = strchr(temp, '='));
       temp += 1) {
    current_ctx->user_envs_count++;
  }

  if (current_ctx->user_envs_count == 0) {
    return -1;
  }

  return 0;
}

/**
 * cleanup_user_envs - Free partially allocated user_envs array
 * @current_ctx: Shell context
 * @i: index where allocation failed
 *
 * Calls when allocation fails partway through parsing, frees any previously
 * allocated entries before the failure point.
 */
void cleanup_user_envs(struct repl_ctx *current_ctx, unsigned int i) {
  for (int j = (int)i - 1; j >= 0; j--) {
    if (current_ctx->user_envs[j].name) {
      free(current_ctx->user_envs[j].name);
    }

    if (current_ctx->user_envs[j].value) {
      free(current_ctx->user_envs[j].value);
    }
  }

  if (current_ctx->user_envs) {
    free(current_ctx->user_envs);
  }

  current_ctx->user_envs = NULL;
}

/**
 * parse_user_envs - Parse config file into user_envs array
 * @current_ctx: Shell context
 * @config_file_contents: File contents as string
 *
 * Parses each line of format NAME=VALUE into user_env structs. Allocates memory
 * for each name and value string.
 *
 * Return: 0 on success, -1 on error
 */
int parse_user_envs(struct repl_ctx *current_ctx, char *config_file_contents) {
  current_ctx->user_envs =
      malloc(current_ctx->user_envs_count * sizeof(struct user_env));
  if (!current_ctx->user_envs) {
    error_msg(malloc_fail_msg, true);
    return -1;
  }

  /* Split string on '\n' to get individual lines */
  char *current_line = strtok(config_file_contents, "\n");
  unsigned int i = 0;

  while (current_line && i < current_ctx->user_envs_count) {
    char *equal_sign = strchr(current_line, '=');

    if (!equal_sign) {
      current_line = strtok(NULL, "\n");
      continue;
    }

    /* Multiple equals signs in a row is bad syntax */
    if (strcmp(equal_sign + 1, "=") == 0) {
      error_msg("Malformed configuration file", false);
      return -1;
    }

    /*
     * Calculate the lengths of each field so we know how many characters to
     * copy to the destination strings.
     */
    size_t name_len = equal_sign - current_line;
    size_t value_len = strlen(equal_sign + 1);

    /* An additional byte is allocated for null terminator */
    current_ctx->user_envs[i].name = malloc(name_len + 1);

    if (!current_ctx->user_envs[i].name) {
      cleanup_user_envs(current_ctx, i);
      error_msg(malloc_fail_msg, true);
      return -1;
    }

    current_ctx->user_envs[i].value = malloc(value_len + 1);
    if (!current_ctx->user_envs[i].value) {
      error_msg(malloc_fail_msg, true);
      cleanup_user_envs(current_ctx, i);
      return -1;
    }

    /*
     * strncpy doesn't always null terminate if it fills the buffer,
     * so we do so explicitly.
     */
    strncpy(current_ctx->user_envs[i].name, current_line, name_len);
    current_ctx->user_envs[i].name[name_len] = '\0';

    strcpy(current_ctx->user_envs[i].value, equal_sign + 1);

    i++;
    current_line = strtok(NULL, "\n");
  }

  return 0;
}

/**
 * load_config - Main config file loading function
 * @current_ctx: Shell context
 *
 * Failing to load the config file is non-fatal, the shell can work fine without
 * it.
 *
 * Return: 0 on success, -1 on error
 */
int load_config(struct repl_ctx *current_ctx) {
  if (construct_config_path(current_ctx) == -1) {
    return -1;
  }

  if (!file_exists(current_ctx->config_filename)) {
    return -1;
  }

  char *config_file_contents = read_file(current_ctx->config_filename);
  if (!config_file_contents) {
    return -1;
  }

  if (count_user_envs(current_ctx, config_file_contents) == -1) {
    free(config_file_contents);
    return -1;
  }

  if (parse_user_envs(current_ctx, config_file_contents) == -1) {
    free(config_file_contents);
    return -1;
  }

  free(config_file_contents);

  return 0;
}

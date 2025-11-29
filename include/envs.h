/**
 * envs.h
 *
 * Provides safe wrappers around environment variable access with fallback
 * values for when variables aren't set.
 */

#ifndef ENVS_H
#define ENVS_H

/**
 * getenv_checked - Get environment vairable with special handling.
 *
 * Wrapper around getenv() that provides a fallback when the variable doesn't
 * exist. This avoid NULL pointer dereferences when we expect a value.
 *
 * Return: Variable value if set, fallback otherwise
 */
char *getenv_checked(char *env, char *fallback_value);

/**
 * init_home_dir - Get user's home directory
 *
 * Retrieves the $HOME environment varable. We need this for a lot of things:
 * - cd with no args goes to $HOME
 * - ~ expands to $HOME
 * - Config file is in $HOME
 * - History file is in $HOME
 *
 * Since we depend on $HOME for so many things, the shell exits if this returns
 * NULL.
 *
 * Return: HOME directory path, NULL if not set
 */
char *init_home_dir(void);

#endif

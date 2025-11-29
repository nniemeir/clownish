/**
 * history.c
 *
 * Command history management using GNU Readline
 *
 * OVERVIEW:
 * Responsible for handling persistent command history.
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <readline/history.h>

#include "error.h"
#include "file.h"
#include "history.h"

/**
 * init_history - Initialize command history at startup
 * @home_dir: User's home directory
 *
 * Sets up the readline history system.
 *
 * Return: Path to history file, NULL on error
 */
char *init_history(char *home_dir) {
  char *hist_file = malloc(PATH_MAX);
  if (!hist_file) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  /* Construct the full path to the history file */
  snprintf(hist_file, PATH_MAX, "%s/.clown_scribbles", home_dir);

  /* Initialize readline history system */
  using_history();

  /* There is no history to load in if the file doesn't exist yet */
  if (!file_exists(hist_file)) {
    return hist_file;
  }

  /* Load previously saved history */
  if (read_history(hist_file) != 0) {
    error_msg("Failed to read history", false);
    free(hist_file);
    return NULL;
  }

  return hist_file;
}

/**
 * close_history - Save and cleanup history at shell exit
 * @hist_file: Path to history file
 *
 * Writes accumulated commands to disk and cleans up history system, allowing us
 * to read them in on next run.
 */
void close_history(char *hist_file) {
  /* Writes the history to file */
  if (write_history(hist_file) != 0) {
    error_msg("Failed to write history", false);
  }

  free(hist_file);

  /* To avoid memory leaks, we free all history entries */
  clear_history();
}

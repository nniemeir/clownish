/**
 * history.h
 *
 * Provides persistent command history across shell sessions using GNU
 * Readline's history facilities.
 * 
 * Commands are saved to ~/.clown_scribbles.
 */

#ifndef HISTORY_H
#define HISTORY_H

/**
 * init_history - Initialize command history at startup
 * @home_dir: User's home directory
 *
 * Sets up the readline history system.
 *
 * Return: Path to history file, NULL on error
 */
char *init_history(char *home_dir);

/**
 * close_history - Save and cleanup history at shell exit
 * @hist_file: Path to history file
 *
 * Writes accumulated commands to disk and cleans up history system, allowing us
 * to read them in on next run.
 */
void close_history(char *hist_file);

#endif

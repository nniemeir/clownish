/**
 * signals.h
 * 
 * Declares functions for configuring how the shell responds to Unix signals.
 */

#ifndef SIGNALS_H
#define SIGNALS_H

/**
 * handler - SIGINT (Ctrl+C) signal handler
 * @signal_num: Signal number (unused, but required by API)
 *
 * Called when user presses Ctrl+C. Readline has its own signal handling for
 * SIGINT, so we just start a fresh line.
 */
void handler(int signal_num);

/**
 * init_sig_handler - Configure signal handlers at startup
 *
 * Return: 0 on success, -1 on error
 */
int init_sig_handler(void);

#endif

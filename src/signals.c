/**
 * signals.c
 *
 * Signal handling configuration.
 *
 * OVERVIEW:
 * Configures how the shell responds to Unix signals:
 * - SIGINT (Ctrl+C): Cancels current line
 * - SIGCHLD: Child process status change - Ignored
 */

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "error.h"
#include "signals.h"

/**
 * handler - SIGINT (Ctrl+C) signal handler
 * @signal_num: Signal number (unused, but required by API)
 *
 * Called when user presses Ctrl+C. Readline has its own signal handling for
 * SIGINT, so we just start a fresh line.
 */
void handler(int signal_num) {
  /* Suppress unused parameter warning */
  (void)signal_num;
  /**
   * write() is async-signal-safe. This is important because SIGINT can be sent
   * at any time during execution.
   */
  write(STDOUT_FILENO, "\n", 1);
}

/**
 * init_sig_handler - Configure signal handlers at startup
 *
 * Return: 0 on success, -1 on error
 */
int init_sig_handler(void) {
  /*
   * SIG_IGN tells kernel to ignore SIGCHLD. This makes it so that the kernel
   * automatically reaps child processes for us.
   */
  signal(SIGCHLD, SIG_IGN);

  struct sigaction sa;

  /*
   * Zero out the entire struct to ensure there are no garbage values. It is
   * good practice to do this before passing to syscalls.
   */
  memset(&sa, 0, sizeof(sa));

  sa.sa_handler = handler;

  /* sigaction() installs our signal handler for SIGINT */
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    error_msg("Failed to configure signal handling", true);
    return -1;
  }

  return 0;
}

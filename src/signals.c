#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "error.h"
#include "signals.h"

void handler(int signal_num) {
  (void)signal_num;
  write(STDOUT_FILENO, "\n", 2);
}

int init_sig_handler(void) {
  signal(SIGCHLD, SIG_IGN);

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));

  sa.sa_handler = handler;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    error_msg("Failed to configure signal handling", true);
    return 1;
  }

  return 0;
}

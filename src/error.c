#include "error.h"
#include "prompt.h"

const char *program_name = "clowniSH";

const char *blame_user_msg = "...likely %s's fault.\n";

const char *env_fail_msg = "Failed to resolve environment variable";

const char *malloc_fail_msg = "Memory allocation failed";

const char *open_fail_msg = "Failed to open file";

const char *close_fail_msg = "Failed to close file descriptor";

const char *dup2_fail_msg = "Failed to duplicate file descriptor";

const char *redirection_missing_filename_msg =
    "No filename provided for redirection";

void error_msg(const char *context, bool report_errno) {
  if (report_errno) {
    fprintf(stderr, "%s%s: %s: %s %s\n", RED, program_name, context,
            strerror(errno), CYAN);
    return;
  }
  fprintf(stderr, "%s%s: %s %s\n", RED, program_name, context, CYAN);
}
/**
 * error.c
 *
 * Error message formatting and common error strings.
 *
 * OVERVIEW:
 * Centralizes error message formatting for consistency. All error messages are
 * color-coded and begin with "clowniSH:".
 *
 * errno is a global variable that system calls set on failure. The
 * sterror(errno) function matches the error code to a string.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "input.h"

const char *program_name = "clowniSH";

const char *blame_user_msg = "...likely %s's fault.\n";

const char *env_fail_msg = "Failed to resolve environment variable";

const char *malloc_fail_msg = "Memory allocation failed";

const char *open_fail_msg = "Failed to open file";

const char *close_fail_msg = "Failed to close file descriptor";

const char *dup2_fail_msg = "Failed to duplicate file descriptor";

const char *strdup_fail_msg = "Failed to duplicate string";

const char *redirection_missing_filename_msg =
    "No filename provided for redirection";

/**
 * error_msg - Print formatted error message
 * @context: Description of what operation failed
 * @report_errno: Whether to include system error string
 *
 * Prints a standardized error message to stderr with color coding, includes
 * errno string when relevant.
 */
void error_msg(const char *context, bool report_errno) {
  if (report_errno) {
    fprintf(stderr, "%s%s: %s: %s %s\n", RED, program_name, context,
            strerror(errno), CYAN);
    return;
  }

  fprintf(stderr, "%s%s: %s %s\n", RED, program_name, context, CYAN);
}

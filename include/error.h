/**
 * error.h
 *
 * Provides centralized error handling with consistent formatting and
 * color-coding.
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

/* ERR_MSG_MAX - Maximum error message length*/
#define ERR_MSG_MAX 2048

/* program_name - Shell's name for error messages*/
extern const char *program_name;

/* blame_user_msg - Format string for blaming user */
extern const char *blame_user_msg;

/* close_fail_msg - Error closing file descriptor */
extern const char *close_fail_msg;

/* dup2_fail_msg - Error duplicating file descriptor */
extern const char *dup2_fail_msg;

/* env_fail_msg - Error resolving environment variable */
extern const char *env_fail_msg;

/* malloc_fail_msg - Memory allocation failure */
extern const char *malloc_fail_msg;

/* open_fail_msg - Error opening file */
extern const char *open_fail_msg;

/* redirection_missing_filename_msg - No filename for redirection */
extern const char *redirection_missing_filename_msg;

/* strdup_fail_msg - String duplication failure */
extern const char *strdup_fail_msg;

/**
 * error_msg - Print formatted error message
 * @context: Description of what operation failed
 * @report_errno: Whether to include system error string
 *
 * Prints a standardized error message to stderr with color coding, includes
 * errno string when relevant.
 */
void error_msg(const char *function_name, bool report_errno);

#endif

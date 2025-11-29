/**
 * parse_utils.c
 *
 * Utility functions for string and array manipulation during parsing.
 *
 * OVERVIEW:
 * Responsible for helper functions used throughout parsing stages.
 */

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "parse.h"

/**
 * remove_arg - Remove argument from command array
 * @command: Array of command arguments
 * @args_count: Pointer to argument count (decremented)
 * @args_index: Index of argument to remove
 *
 * Removes an argument from the command array by shifting all subsequent
 * elements down by one position. This is used to remove shell syntax elements
 * before executing programs.
 */
void remove_arg(char **command, unsigned int *args_count,
                unsigned int arg_index) {
  if (arg_index == 0) {
    error_msg("You tried to remove the program name from a command", false);
    return;
  }

  if (arg_index >= *args_count) {
    error_msg("You attempted to remove more elements from an array than it had",
              false);
    return;
  }

  /*
   * Shift all elements after arg_index down by one position, overwriting the
   * element at arg_index.
   */
  for (unsigned int i = arg_index; i < *args_count - 1; i++) {
    command[i] = command[i + 1];
  }

  /* Null-terminate at the new end of the array */
  command[*args_count - 1] = NULL;
  (*args_count)--;
}

/**
 * replace - Find and replace substring in string
 * @original_str: Pointer to string to modify (modified in-place)
 * @original_substr: Substring to find
 * @new_substr: Substring to replace with
 *
 * Replaces all occurrences of original_substr with new_substr.
 */
void replace(char **original_str, const char *original_substr,
             const char *new_substr) {
  int original_len = strlen(original_substr);
  int new_len = strlen(new_substr);
  unsigned int match_count = 0;

  /* Count occurrences of original_substr */
  for (const char *temp = *original_str; (temp = strstr(temp, original_substr));
       temp += original_len) {
    match_count++;
  }

  /* Calculate required size for the new string */
  int new_str_len =
      strlen(*original_str) + match_count * (new_len - original_len) + 1;

  char *new_str = malloc(new_str_len);
  if (!new_str) {
    error_msg(malloc_fail_msg, true);
    return;
  }

  const char *src = *original_str;
  char *dst = new_str;

  /* Build new string character by character */
  while (*src) {
    if (strncmp(src, original_substr, original_len) == 0) {
      /* Found match, copy replacement string */
      strcpy(dst, new_substr);
      src += original_len;
      dst += new_len;
    } else {
      /* No match, copy single character */
      *dst++ = *src++;
    }
  }

  /* Null-terminate the new string */
  *dst = '\0';

  /* Copy result back to original string */
  strcpy(*original_str, new_str);

  free(new_str);
}

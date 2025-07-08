#include "error.h"
#include "parse.h"

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

  for (unsigned int i = arg_index; i < *args_count - 1; i++) {
    command[i] = command[i + 1];
  }

  command[*args_count - 1] = NULL;
  (*args_count)--;

  return;
}

void replace(char **original_str, const char *original_substr,
             const char *new_substr) {
  int original_len = strlen(original_substr);
  int new_len = strlen(new_substr);
  unsigned int match_count = 0;

  for (const char *temp = *original_str; (temp = strstr(temp, original_substr));
       temp += original_len) {
    match_count++;
  }

  int new_str_len =
      strlen(*original_str) + match_count * (new_len - original_len) + 1;

  char *new_str = malloc(new_str_len);
  if (!new_str) {
    error_msg(malloc_fail_msg, true);
    return;
  }

  const char *src = *original_str;
  char *dst = new_str;
  while (*src) {
    if (!strncmp(src, original_substr, original_len)) {
      strcpy(dst, new_substr);
      src += original_len;
      dst += new_len;
    } else {
      *dst++ = *src++;
    }
  }

  *dst = '\0';

  strcpy(*original_str, new_str);

  free(new_str);
}

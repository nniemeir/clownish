#ifndef PARSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 64
#define DELIM " \t\r\n\a"
#define NULL_TERMINATOR_LENGTH 1
#define PROMPT_MAX (_SC_HOST_NAME_MAX + _SC_LOGIN_NAME_MAX + PATH_MAX)

char *replace(const char *original_str, const char *original_substr,
              const char *new_substr);
char *parse_envs(const char *input);
char **tokenize_input(char *line, int *args_count);
#endif
#include "error.h"
#include "parse.h"

char **lex_input(char *line, unsigned int *args_count) {
  static const char *delim = " \t\r\n\a";

  int buffer_size = TOKENS_MAX;

  char **tokens = malloc(buffer_size * sizeof(char *));
  if (!tokens) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  char *token;
  token = strtok(line, delim);

  while (token) {
    tokens[*args_count] = strdup(token);
    if (!tokens[*args_count]) {
      error_msg(strdup_fail_msg, true);
      for (unsigned int i = 0; i < *args_count; i++) {
        free(tokens[i]);
      }
      free(tokens);
      return NULL;
    }

    (*args_count)++;

    if (*args_count >= (unsigned int)buffer_size) {
      buffer_size += TOKENS_MAX;

      tokens = realloc(tokens, buffer_size * sizeof(char *));

      if (!tokens) {
        error_msg("Failed to reallocate memory", true);
        return NULL;
      }
    }

    token = strtok(NULL, delim);
  }

  tokens[*args_count] = NULL;

  return tokens;
}

bool pipes_exceeded(const char *line, unsigned int *commands_count) {
  int pipe_count = 0;

  char *temp = strdup(line);
  if (!temp) {
    error_msg(strdup_fail_msg, true);
    return true;
  }

  char *temp_start = temp;

  while ((temp = strstr(temp, " | "))) {
    pipe_count++;
    temp += 3;
  }

  free(temp_start);

  if (pipe_count >= PIPES_MAX) {
    error_msg("Number of commands exceeded", false);
    return true;
  }

  *commands_count = pipe_count + 1;

  return false;
}

char **split_on_pipes(const char *line, unsigned int *commands_count) {
  if (pipes_exceeded(line, commands_count)) {
    return NULL;
  }

  char **unparsed_commands = malloc(*commands_count * sizeof(char *));
  if (!unparsed_commands) {
    return NULL;
  }

  char *input = strdup(line);
  if (!input) {
    error_msg(strdup_fail_msg, true);
    return NULL;
  }

  char *start = input;
  char *position;
  unsigned int index = 0;

  while ((position = strstr(start, " | ")) && index < PIPES_MAX) {
    *position = '\0';

    unparsed_commands[index] = strdup(start);
    if (!unparsed_commands[index]) {
      error_msg(strdup_fail_msg, true);
      free(input);
      for (int i = index - 1; i >= 0; i--) {
        free(unparsed_commands[i]);
      }
      free(unparsed_commands);
      return NULL;
    }

    start = position + 3;
    index++;
  }

  if (index == PIPES_MAX) {
    free(input);
    return NULL;
  }

  unparsed_commands[index++] = strdup(start);
  if (!unparsed_commands[index - 1]) {
    error_msg(strdup_fail_msg, true);
    free(input);
    for (int i = index - 1; i >= 0; i--) {
      free(unparsed_commands[i]);
    }
    free(unparsed_commands);
    return NULL;
  }

  *commands_count = index;

  free(input);

  return unparsed_commands;
}

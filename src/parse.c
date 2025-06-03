#include "../include/parse.h"

char **split_input(char *line, int *args_count) {
  int buffer_size = TOKEN_BUFFER_SIZE;
  char **tokens = malloc(buffer_size * sizeof(char *));
  char *token;

  if (!tokens) {
    printf("clowniSH: Failed to allocate memory for token buffer.\n");
    return NULL;
  }

  token = strtok(line, TOKEN_DELIM);
  while (token) {
    tokens[*args_count] = token;
    (*args_count)++;

    if (*args_count >= buffer_size) {
      buffer_size += TOKEN_BUFFER_SIZE;
      tokens = realloc(tokens, buffer_size * sizeof(char *));
      if (!tokens) {
        printf("clowniSH: Failed to allocate memory for token buffer.\n");
        return NULL;
      }
    }

    token = strtok(NULL, TOKEN_DELIM);
  }
  tokens[*args_count] = NULL;
  return tokens;
}

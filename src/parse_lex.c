/**
 * parse_lex.c
 *
 * Lexical analysis (tokenization) for shell input.
 *
 * OVERVIEW:
 * Responsible for handling breaking user input into individual tokens (words).
 * It also handles splitting command pipelines on the pipe operator (|).
 *
 * We split on pipes before lexing because we need to know how many commands
 * there are before we allocate arrays for their arguments and I/O redirections.
 */

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "parse.h"

/**
 * lex_input - Tokenize a single command into arguments
 * @line: Command string to tokenize (modified by strtok)
 * @args_count: Output parameter - number of arguments found
 *
 * Uses strtok() to split on whitespace (spaces, tabs, newlines). Each token is
 * duplicated with strdup so it persists after strtok modifies the original
 * string.
 *
 * Return: Array of strings (NULL-terminated), NULL on error
 */
char **lex_input(char *line, unsigned int *args_count) {
  /* Delimiters: space, tab, carriage return, newline, bell */
  static const char *delim = " \t\r\n\a";

  int buffer_size = TOKENS_MAX;

  char **tokens = malloc(buffer_size * sizeof(char *));
  if (!tokens) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  char *token;
  /**
   * strtok() breaks a string into tokens.
   * The first call to it (strtok(string, delims)) returns first token.
   * Subsequent calls (strok(NULL, delims)) returns the next token.
   * Returns NULL when no more tokens are found.
   */
  token = strtok(line, delim);

  while (token) {
    /**
     * We duplicate the string with strdup because strtok's tokens are just
     * pointers into the original string, which we'll free soon.
     */
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

    /* Grow the buffer if we're running out of space */
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

  /* Null-terminate the array (required by execvp) */
  tokens[*args_count] = NULL;

  return tokens;
}

/**
 * pipes_exceeded - Check if command has too many pipes
 * @line: Full command string
 * @commands_count: Ouput parameter - number of commands found
 *
 * Pipes consume file descriptors and memory, so we limit the number of pipes to
 * PIPES_MAX (64).
 *
 * Return: true if exceeded, false otherwise
 */
bool pipes_exceeded(const char *line, unsigned int *commands_count) {
  int pipe_count = 0;

  char *temp = strdup(line);
  if (!temp) {
    error_msg(strdup_fail_msg, true);
    return true;
  }

  char *temp_start = temp;

  /**
   * We search for " | " rather than just "|" in case a file has the pipe symbol
   * in its name.
   */
  while ((temp = strstr(temp, " | "))) {
    pipe_count++;
    /* Move past " | " */
    temp += 3;
  }

  free(temp_start);

  if (pipe_count >= PIPES_MAX) {
    error_msg("Number of commands exceeded", false);
    return true;
  }

  /* The last command won't have a pipe after it */
  *commands_count = pipe_count + 1;

  return false;
}

/**
 * split_on_pipes - Split command line into individual commands
 * @line: Full command string
 * @commands_count: Output parameter - number of commands found
 *
 * Returns: Array of command strings, NULL on error
 */
char **split_on_pipes(const char *line, unsigned int *commands_count) {
  if (pipes_exceeded(line, commands_count)) {
    return NULL;
  }

  char **unparsed_commands = malloc(*commands_count * sizeof(char *));
  if (!unparsed_commands) {
    return NULL;
  }

  /* Duplicate the input string so that we can modify it */
  char *input = strdup(line);
  if (!input) {
    error_msg(strdup_fail_msg, true);
    return NULL;
  }

  char *start = input;
  char *position;
  unsigned int index = 0;

  /**
   * Find each " | " seperator and replace it with null terminator to split the
   * string. Duplicate the command substring and save it.
   */
  while ((position = strstr(start, " | ")) && index < PIPES_MAX) {
    *position = '\0';

    unparsed_commands[index] = strdup(start);
    if (!unparsed_commands[index]) {
      error_msg(strdup_fail_msg, true);
      free(input);
      /* Clean up commands that we've already allocated */
      for (int i = index - 1; i >= 0; i--) {
        free(unparsed_commands[i]);
      }
      free(unparsed_commands);
      return NULL;
    }

    /* Move past " | " */
    start = position + 3;
    index++;
  }

  /* This is overly cautious as pipes_exceeded checks this */
  if (index == PIPES_MAX) {
    free(input);
    return NULL;
  }

  /* Save the last command separately since it doesn't end with a pipe */
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

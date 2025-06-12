#include "../include/parse.h"
#include "../include/prompt.h"

char *replace(const char *original_str, const char *original_substr,
              const char *new_substr) {
  int original_len = strlen(original_substr), new_len = strlen(new_substr);
  int count = 0;
  for (const char *temp = original_str; (temp = strstr(temp, original_substr));
       temp += original_len) {
    count++;
  }
  int new_str_len = strlen(original_str) + count * (new_len - original_len) + 1;
  char *new_str = malloc(new_str_len);
  if (!new_str) {
    fprintf(stderr, "clowniSH: Failed to allocate memory for new_str.\n");
    return NULL;
  }
  const char *src = original_str;
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
  return new_str;
}

int check_if_background(char *input) {
  size_t len = strlen(input);
  if (len >= 2 && strcmp(input + len - 2, " &") == 0) {
    input[len - 2] = '\0';
    return 1;
  }
  return 0;
}

int parse_envs(const char *input, char **parsed_str_buffer) {
  strcpy(*parsed_str_buffer, input);
  char temp[PROMPT_MAX];
  char *start = *parsed_str_buffer;
  while ((start = strstr(start, "$")) != NULL) {
    char var_name[ARG_MAX];
    int i = 0;
    start++;
    while (*start && (isalnum(*start) || *start == '_')) {
      var_name[i++] = *start++;
    }
    var_name[i] = '\0';
    char *env_value = getenv(var_name);
    if (!env_value) {
      fprintf(stderr, "clowniSH: Failed to resolve %s.\n", var_name);
      env_value = "";
    }
    strncpy(temp, *parsed_str_buffer,
            start - *parsed_str_buffer - i - NULL_TERMINATOR_LENGTH);
    temp[start - *parsed_str_buffer - i - NULL_TERMINATOR_LENGTH] = '\0';
    strcat(temp, env_value);
    strcat(temp, start);

    strcpy(*parsed_str_buffer, temp);
    start = *parsed_str_buffer;
  }
  return 1;
}

char **tokenize_input(char *line, int *args_count) {
  int buffer_size = BUFFER_SIZE;
  char **tokens = malloc(buffer_size * sizeof(char *));
  char *token;
  if (!tokens) {
    printf("clowniSH: Failed to allocate memory for token buffer.\n");
    return NULL;
  }
  token = strtok(line, DELIM);
  while (token) {
    tokens[*args_count] = token;
    (*args_count)++;
    if (*args_count >= buffer_size) {
      buffer_size += BUFFER_SIZE;
      tokens = realloc(tokens, buffer_size * sizeof(char *));
      if (!tokens) {
        printf("clowniSH: Failed to allocate memory for token buffer.\n");
        return NULL;
      }
    }
    token = strtok(NULL, DELIM);
  }
  tokens[*args_count] = NULL;
  return tokens;
}

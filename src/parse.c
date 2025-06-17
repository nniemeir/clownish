#include "../include/parse.h"
#include "../include/prompt.h"

void remove_arg(char **args, unsigned int *args_count, unsigned int arg_index) {
  if (arg_index >= *args_count) {
    fprintf(stderr, "clowniSH: Invalid index passed to remove_arg.\n");
    return;
  }
  if (arg_index == 0) {
    fprintf(stderr,
            "clowniSH: Removing command name from args is forbidden.\n");
    return;
  }
  for (unsigned int i = arg_index; i < *args_count - 1; i++) {
    args[i] = args[i + 1];
  }
  args[*args_count - 1] = NULL;
  (*args_count)--;
  return;
}

void replace(char **original_str, const char *original_substr,
             const char *new_substr) {
  int original_len = strlen(original_substr), new_len = strlen(new_substr);
  int count = 0;
  for (const char *temp = *original_str; (temp = strstr(temp, original_substr));
       temp += original_len) {
    count++;
  }
  int new_str_len =
      strlen(*original_str) + count * (new_len - original_len) + 1;
  char *new_str = malloc(new_str_len);
  if (!new_str) {
    fprintf(stderr, "clowniSH: Failed to allocate memory for new_str.\n");
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

void check_if_background(struct repl_ctx *current_ctx) {
  if (strcmp(current_ctx->args[current_ctx->args_count - 1], "&") == 0) {
    current_ctx->is_background_process = 1;
    remove_arg(current_ctx->args, &current_ctx->args_count,
               current_ctx->args_count - 1);
  }
}

void determine_out_stream(struct repl_ctx *current_ctx) {
  current_ctx->out_stream_type = 0;
  strcpy(current_ctx->out_stream_name, "");
  char *gt;
  char *two_gt;
  for (unsigned int i = 0; i < current_ctx->args_count; i++) {
    gt = strstr(current_ctx->args[i], ">");
    if (gt) {
      current_ctx->out_stream_type = O_WRONLY;
      strcpy(current_ctx->out_stream_name, current_ctx->args[i + 1]);
      // Called twice to remove gt and the stream name
      remove_arg(current_ctx->args, &current_ctx->args_count, i);
      remove_arg(current_ctx->args, &current_ctx->args_count, i);
      return;
    }
    two_gt = strstr(current_ctx->args[i], ">>");
    if (two_gt) {
      current_ctx->out_stream_type = O_APPEND;
      strcpy(current_ctx->out_stream_name, current_ctx->args[i + 1]);
      remove_arg(current_ctx->args, &current_ctx->args_count, i);
      remove_arg(current_ctx->args, &current_ctx->args_count, i);
      return;
    }
  }
}

int parse_envs(char **arg) {
  char *start = *arg;
  if (strstr(start, "$")) {
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
    *arg = env_value;
  }
  return 1;
}

char **tokenize_input(char *line, unsigned int *args_count) {
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
    if (*args_count >= (unsigned int)buffer_size) {
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

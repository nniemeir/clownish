#include "parse.h"
#include "error.h"

void remove_arg(char **args, unsigned int *args_count, unsigned int arg_index) {
  if (arg_index == 0) {
    fprintf(stderr, "Do you really think passing an args array to execvp "
                    "without a command name is a good idea?\n");
    return;
  }

  if (arg_index >= *args_count) {
    fprintf(stderr,
            "You can't remove the %uth element of an args array with only %u "
            "elements.\n",
            arg_index, *args_count - 1);
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
    fprintf(stderr, malloc_fail_msg, "new_str");
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
  current_ctx->is_background_process = 0;
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
    char var_name[ENV_MAX];
    int i = 0;
    start++;
    while (*start && (isalnum(*start) || *start == '_')) {
      var_name[i++] = *start++;
    }
    var_name[i] = '\0';
    char *env_value = getenv(var_name);
    if (!env_value) {
      fprintf(stderr, env_fail_msg, var_name);
      env_value = "";
    }
    *arg = env_value;
  }
  return 1;
}

char **tokenize_input(char *line, unsigned int *args_count) {
  int buffer_size = TOKENS_MAX;
  char **tokens = malloc(buffer_size * sizeof(char *));
  if (!tokens) {
    fprintf(stderr, malloc_fail_msg, "token buffer");
    return NULL;
  }

  char *token;
  token = strtok(line, DELIM);

  while (token) {
    tokens[*args_count] = token;
    (*args_count)++;

    if (*args_count >= (unsigned int)buffer_size) {
      buffer_size += TOKENS_MAX;
      tokens = realloc(tokens, buffer_size * sizeof(char *));

      if (!tokens) {
        fprintf(stderr, malloc_fail_msg, "token buffer");
        return NULL;
      }
    }

    token = strtok(NULL, DELIM);
  }

  tokens[*args_count] = NULL;
  return tokens;
}

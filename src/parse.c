#include "parse.h"
#include "config.h"
#include "error.h"

static const char *delim = " \t\r\n\a";

#define ERR_MSG_MAX 2048

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

void determine_if_background(struct repl_ctx *current_ctx,
                             unsigned int command_index) {
  current_ctx->is_background_process = 0;
  if (strcmp(current_ctx->commands[command_index]
                                  [current_ctx->args_count[command_index] - 1],
             "&") == 0) {
    if (command_index - 1 != current_ctx->commands_count) {
      error_msg("Background operator can only be specified for the last "
                "command in the pipeline.",
                false);
      return;
    }
    current_ctx->is_background_process = 1;
    remove_arg(current_ctx->commands[command_index],
               &current_ctx->args_count[command_index],
               current_ctx->args_count[command_index] - 1);
  }
}

void determine_in_stream(struct repl_ctx *current_ctx,
                         unsigned int command_index) {
  for (unsigned int i = 0; i < current_ctx->args_count[command_index]; i++) {
    if (strcmp(current_ctx->commands[command_index][i], "<") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }
      current_ctx->in_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->in_stream_name[command_index]) {
        error_msg("Failed to duplicate string", true);
        return;
      }

      // Called twice to remove lt and the stream name
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      return;
    }
  }
}

void determine_out_stream(struct repl_ctx *current_ctx,
                          unsigned int command_index) {
  for (unsigned int i = 0; i < current_ctx->args_count[command_index]; i++) {
    if (strcmp(current_ctx->commands[command_index][i], ">") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }
      current_ctx->out_stream_type[command_index] = O_WRONLY;
      current_ctx->out_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->out_stream_name[command_index]) {
        error_msg("Failed to duplicate string", true);
        return;
      }
      // Called twice to remove gt and the stream name
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      return;
    }

    if (strcmp(current_ctx->commands[command_index][i], ">>") == 0) {
      if (!current_ctx->commands[command_index][i + 1]) {
        error_msg(redirection_missing_filename_msg, false);
        return;
      }
      current_ctx->out_stream_type[command_index] = O_APPEND;
      current_ctx->out_stream_name[command_index] =
          strdup(current_ctx->commands[command_index][i + 1]);
      if (!current_ctx->out_stream_name[command_index]) {
        error_msg("Failed to duplicate string", true);
        return;
      }
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      remove_arg(current_ctx->commands[command_index],
                 &current_ctx->args_count[command_index], i);
      return;
    }
  }
}

void parse_envs(char **arg, struct user_env *user_envs,
                unsigned int user_envs_count) {
  char *start = *arg;
  if (strstr(start, "$")) {
    char var_name[ENV_MAX];
    unsigned int i = 0;
    start++;
    while (*start && (isalnum(*start) || *start == '_')) {
      var_name[i++] = *start++;
    }
    var_name[i] = '\0';
    char *env_value;
    if (user_envs_count > 0) {
      env_value = get_user_env(var_name, user_envs, user_envs_count);
      if (env_value) {
        *arg = env_value;
        return;
      }
    }
    env_value = getenv(var_name);
    if (!env_value) {
      error_msg(env_fail_msg, false);
      env_value = "";
      return;
    }

    *arg = strdup(env_value);
    if (!*arg) {
      error_msg("Failed to duplicate string", true);
    }
    return;
  }
}

bool pipes_exceeded(const char *line, unsigned int *commands_count) {
  int pipe_count = 0;
  char *temp = strdup(line);
  if (!temp) {
    error_msg("Failed to duplicate string", true);
    return true;
  }
  char *temp_start = temp;
  while ((temp = strstr(temp, " | "))) {
    pipe_count++;
    temp += 3;
  }
  free(temp_start);
  if (pipe_count >= PIPES_MAX) {
    error_msg("Failed to duplicate string", true);
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
    error_msg("Failed to duplicate string", true);
    return NULL;
  }
  char *start = input;
  char *position;
  unsigned int index = 0;
  while ((position = strstr(start, " | ")) && index < PIPES_MAX) {
    *position = '\0';
    unparsed_commands[index] = strdup(start);
    if (!unparsed_commands[index]) {
      error_msg("Failed to duplicate string", true);
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
    error_msg("Failed to duplicate string", true);
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

char **tokenize_input(char *line, unsigned int *args_count) {
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
      error_msg("Failed to duplicate string", true);
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

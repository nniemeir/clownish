#include "config.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "parse.h"

char *get_user_env(const char *var_name, struct user_env *user_envs,
                   unsigned int user_envs_count) {
  for (unsigned int i = 0; i < user_envs_count; i++) {
    if (strcmp(var_name, user_envs[i].name) == 0) {
      return user_envs[i].value;
    }
  }
  return NULL;
}

int construct_config_path(struct repl_ctx *current_ctx) {
  current_ctx->config_filename = malloc(PATH_MAX);
  if (!current_ctx->config_filename) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }
  snprintf(current_ctx->config_filename, PATH_MAX, "%s/.clownrc",
           current_ctx->home_dir);
  return 0;
}

int count_user_envs(struct repl_ctx *current_ctx, char *config_file_contents) {
  current_ctx->user_envs_count = 0;

  for (const char *temp = config_file_contents; (temp = strchr(temp, '='));
       temp += 1) {
    current_ctx->user_envs_count++;
  }

  if (current_ctx->user_envs_count == 0) {
    return 1;
  }
  return 0;
}

void cleanup_user_envs(struct repl_ctx *current_ctx, unsigned int i) {
  for (int j = (int)i - 1; j >= 0; j--) {
    if (current_ctx->user_envs[j].name) {
      free(current_ctx->user_envs[j].name);
    }
    if (current_ctx->user_envs[j].value) {
      free(current_ctx->user_envs[j].value);
    }
  }
  if (current_ctx->user_envs) {
    free(current_ctx->user_envs);
  }
  current_ctx->user_envs = NULL;
}

int parse_user_envs(struct repl_ctx *current_ctx, char *config_file_contents) {
  current_ctx->user_envs =
      malloc(current_ctx->user_envs_count * sizeof(struct user_env));

  if (!current_ctx->user_envs) {
    error_msg(malloc_fail_msg, true);
    return 1;
  }

  char *current_line = strtok(config_file_contents, "\n");
  unsigned int i = 0;

  while (current_line && i < current_ctx->user_envs_count) {
    char *equal_sign = strchr(current_line, '=');
    if (!equal_sign) {
      current_line = strtok(NULL, "\n");
      continue;
    }

    if (strcmp(equal_sign + 1, "=") == 0) {
      error_msg("Malformed configuration file", false);
      return 1;
    }

    size_t name_len = equal_sign - current_line;
    size_t value_len = strlen(equal_sign + 1);

    current_ctx->user_envs[i].name = malloc(name_len + 1);
    if (!current_ctx->user_envs[i].name) {
      cleanup_user_envs(current_ctx, i);
      error_msg(malloc_fail_msg, true);
      return 1;
    }
    current_ctx->user_envs[i].value = malloc(value_len + 1);
    if (!current_ctx->user_envs[i].value) {
      error_msg(malloc_fail_msg, true);
      cleanup_user_envs(current_ctx, i);
      return 1;
    }
    strncpy(current_ctx->user_envs[i].name, current_line, name_len);
    current_ctx->user_envs[i].name[name_len] = '\0';
    strcpy(current_ctx->user_envs[i].value, equal_sign + 1);

    i++;
    current_line = strtok(NULL, "\n");
  }
  return 0;
}

int load_config(struct repl_ctx *current_ctx) {
  if (construct_config_path(current_ctx) == 1) {
    return 1;
  }

  if (!file_exists(current_ctx->config_filename)) {
    return 1;
  }

  char *config_file_contents = read_file(current_ctx->config_filename);
  if (!config_file_contents) {
    return 1;
  }

  if (count_user_envs(current_ctx, config_file_contents) == 1) {
    free(config_file_contents);
    return 1;
  }

  if (parse_user_envs(current_ctx, config_file_contents) == 1) {
    free(config_file_contents);
    return 1;
  }

  free(config_file_contents);
  return 0;
}

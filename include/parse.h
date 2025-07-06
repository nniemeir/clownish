#ifndef PARSE_H
#define PARSE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <readline/readline.h>

#define NULL_TERMINATOR_LENGTH 1
#define COMMANDS_MAX 1024
#define ENV_MAX 4096
#define PIPES_MAX 64
#define PROMPT_MAX (_SC_HOST_NAME_MAX + _SC_LOGIN_NAME_MAX + PATH_MAX)
#define TOKENS_MAX 64

struct user_env {
  char *name;
  char *value;
};

struct repl_ctx {
  char *home_dir;
  char *config_filename;
  struct user_env *user_envs;
  unsigned int user_envs_count;
  char *user;
  int receiving;
  char *input;
  char ***commands;
  char **unparsed_commands;
  unsigned int commands_count;
  unsigned int *args_count;
  int is_background_process;
  char **in_stream_name;
  int *in_stream_type;
  char **out_stream_name;
  int *out_stream_type;
};

void remove_arg(char **args, unsigned int *args_count, unsigned int arg_index);
void replace(char **original_str, const char *original_substr,
             const char *new_substr);
void determine_if_background(struct repl_ctx *current_ctx, unsigned int command_index);
void determine_in_stream(struct repl_ctx *current_ctx, unsigned int command_index);
void determine_out_stream(struct repl_ctx *current_ctx, unsigned int command_index);
void parse_envs(char **arg, struct user_env *user_envs, unsigned int user_envs_count);
char **split_on_pipes(const char *line, unsigned int *commands_count);
char **tokenize_input(char *line, unsigned int *args_count);

#endif
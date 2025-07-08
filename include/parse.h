#ifndef PARSE_H
#define PARSE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <readline/readline.h>
#include "context.h"

#define NULL_TERMINATOR_LENGTH 1
#define COMMANDS_MAX 1024
#define ENV_MAX 4096
#define PIPES_MAX 64
#define PROMPT_MAX (_SC_HOST_NAME_MAX + _SC_LOGIN_NAME_MAX + PATH_MAX)
#define TOKENS_MAX 64

void remove_arg(char **args, unsigned int *args_count, unsigned int arg_index);
void replace(char **original_str, const char *original_substr,
             const char *new_substr);
void determine_if_background(struct repl_ctx *current_ctx, unsigned int command_index);
void determine_in_stream(struct repl_ctx *current_ctx, unsigned int command_index);
void determine_out_stream(struct repl_ctx *current_ctx, unsigned int command_index);
void parse_envs(char **arg, struct user_env *user_envs, unsigned int user_envs_count);
char **split_on_pipes(const char *line, unsigned int *commands_count);
char **lex_input(char *line, unsigned int *args_count);

#endif

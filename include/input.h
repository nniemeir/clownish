#ifndef INPUT_H
#define INPUT_H

#include "parse.h"
#include <linux/limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>

#define CYAN "\033[36m"
#define RED "\x1b[31m"
#define WHITE "\x1b[37m"
#define YELLOW "\x1b[33m"

int take_input(struct repl_ctx *current_ctx);
int process_input(struct repl_ctx *current_ctx);
char *construct_prompt(char *home_dir, char *user);
int init_repl_vars(struct repl_ctx *current_ctx);

#endif
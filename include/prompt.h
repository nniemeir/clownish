#ifndef PROMPT_H
#define PROMPT_H

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

int construct_prompt(char **prompt, char *home_dir, char *user);
int prompt_loop(struct repl_ctx *current_ctx);

#endif
#ifndef PROMPT_H
#define PROMPT_H

#include "../include/parse.h"
#include <linux/limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>

#define WHT "\x1b[37m"
#define YEL "\x1b[33m"
#define RED "\x1b[31m"

int construct_prompt(char **prompt, char *home_dir, char *user);
int prompt_loop(struct repl_ctx *current_ctx);

#endif
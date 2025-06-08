#ifndef PROMPT_H
#include <linux/limits.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>

#define WHT "\x1b[37m"
#define YEL "\x1b[33m"
#define RED "\x1b[31m"

int prompt_loop(char ***args, char **input, int *args_count);
#endif
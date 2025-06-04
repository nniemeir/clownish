#ifndef PROMPT_H
#include <linux/limits.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>
int prompt_loop(char ***args, char **input, int *args_count);
#endif
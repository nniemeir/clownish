#ifndef PROMPT_H
#define PROMPT_H
#include "../include/stream.h"
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>

#define WHT "\x1b[37m"
#define YEL "\x1b[33m"
#define RED "\x1b[31m"

int prompt_loop(char ***args, char **input, int *args_count,
                int *is_background_process,
                struct stream_info *current_stream_info);

#endif
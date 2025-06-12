#ifndef EXEC_H
#define EXEC_H
#include "../include/stream.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
int exec(char **args, struct stream_info *current_stream_info,
         int is_background_process);
int exec_builtin(char **args, int *receiving, char *home_dir);
#endif
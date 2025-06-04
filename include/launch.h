#ifndef LAUNCH_H
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int exec(char **args, int *args_count);
int exec_builtin(char **args, int *receiving);

#endif
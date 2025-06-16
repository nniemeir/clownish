#ifndef EXEC_H
#define EXEC_H
#include "../include/stream.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
int exec(struct repl_ctx *current_ctx, int *receiving);
#endif
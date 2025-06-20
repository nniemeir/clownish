#ifndef EXEC_H
#define EXEC_H

#include "../include/parse.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define NUM_OF_BUILTINS 4

struct command_associations {
  char command_name[255];
  int (*command_function)(struct repl_ctx *);
};

int exec(struct repl_ctx *current_ctx);

#endif
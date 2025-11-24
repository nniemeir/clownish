#ifndef EXEC_H
#define EXEC_H

#include "context.h"

#define NUM_OF_BUILTINS 5

struct command_associations {
  char command_name[255];
  int (*command_function)(struct repl_ctx *);
};

int exec(struct repl_ctx *current_ctx);

#endif

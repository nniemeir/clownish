#ifndef BUILTINS_H
#define BUILTINS_H

#include "context.h"

int cat(struct repl_ctx *current_ctx);
int cd(struct repl_ctx *current_ctx);
int cler(struct repl_ctx *current_ctx);
int exit_builtin(struct repl_ctx *current_ctx);
int help(struct repl_ctx *current_ctx);

#endif

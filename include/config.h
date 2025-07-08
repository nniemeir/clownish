#ifndef CONFIG_H
#define CONFIG_H

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"

extern bool debug_mode;

char *get_user_env(const char *var_name, struct user_env *user_envs,
                   unsigned int user_envs_count);
int load_config(struct repl_ctx *current_ctx);

#endif

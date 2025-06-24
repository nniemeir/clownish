#ifndef ENVS_H
#define ENVS_H

#include <stdio.h>
#include <stdlib.h>

char *init_home_dir(void);
char *get_env_s(char *env, char *fallback_value);

#endif
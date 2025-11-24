#ifndef ENVS_H
#define ENVS_H

char *getenv_checked(char *env, char *fallback_value);
char *init_home_dir(void);

#endif

#include "envs.h"

char *getenv_checked(char *env, char *fallback_value) {
  char *env_value = getenv(env);
  if (!env_value) {
    fprintf(stderr, "I failed to resolve %s, so I'll just assume it is %s.\n",
            env, fallback_value);
    env_value = env;
  }
  return env_value;
}

char *init_home_dir(void) {
  char *home_dir = getenv("HOME");
  if (!home_dir) {
    fprintf(stderr, "I failed to resolve HOME, I'm going to take a nap.\n");
    return NULL;
  }
  return home_dir;
}
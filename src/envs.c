#include "envs.h"
#include "error.h"

char *getenv_checked(char *env, char *fallback_value) {
  char *env_value = getenv(env);
  if (!env_value) {
    error_msg("Failed to resolve $USER, so I'll call you Keith", false);
    env_value = fallback_value;
  }
  return env_value;
}

char *init_home_dir(void) {
  char *home_dir = getenv("HOME");
  if (!home_dir) {
    error_msg(env_fail_msg, false);
    return NULL;
  }
  return home_dir;
}

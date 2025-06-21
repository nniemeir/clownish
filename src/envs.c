#include "../include/envs.h"

char *init_home_dir(void) {
  char *home_dir = getenv("HOME");
  if (!home_dir) {
    fprintf(stderr, "clowniSH: Failed to resolve HOME.\n");
    return NULL;
  }
  return home_dir;
}

char *init_user(void) {
  char *user = getenv("USER");
  if (!user) {
    fprintf(stderr, "clowniSH: Failed to resolve USER. I'll just call you Keith.\n");
    user = "keith";
  }
  return user;
}
#include "../include/home.h"
#include <stdio.h>
#include <stdlib.h>

char *init_home_dir(void) {
  char *home_dir = getenv("HOME");
  if (!home_dir) {
    fprintf(stderr, "clowniSH: Failed to resolve HOME.\n");
    return NULL;
  }
  return home_dir;
}
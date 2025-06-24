#ifndef TEASE_H
#define TEASE_H

#include "../include/parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#define NUM_OF_BLACKLISTED_PROGRAMS 3
#define NUM_OF_KNOWN_PROGRAMS 14
#define NUM_OF_KNOWN_TERMINALS 2
#define NUM_OF_KNOWN_DESKTOPS 11

struct joke {
  char name[255];
  char comment[255];
};

extern int teasing_enabled;

void tease_kernel(void);
void tease_terminal(void);
void tease_desktop(void);
int program_is_blacklisted(const char *program_name);
void tease_program(const char *program_name);

#endif
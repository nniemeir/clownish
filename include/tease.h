#ifndef TEASE_H
#define TEASE_H
#include "../include/stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_OF_BLACKLISTED_PROGRAMS 2
#define NUM_OF_KNOWN_PROGRAMS 14
#define NUM_OF_KNOWN_TERMINALS 2
struct jokes {
  char name[255];
  char comment[255];
};
void tease_terminal(void);
int program_is_blacklisted(const char *program_name);
void tease_program(const char *program_name);
extern int teasing_enabled;
#endif
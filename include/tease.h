#ifndef TEASE_H
#define TEASE_H

#include "stdbool.h"

#include "context.h"

#define NUM_OF_BLACKLISTED_PROGRAMS 3
#define NUM_OF_KNOWN_DESKTOPS 58
#define NUM_OF_KNOWN_DISTROS 64
#define NUM_OF_KNOWN_PROGRAMS 43
#define NUM_OF_KNOWN_TERMINALS 2

#define ETHICAL_QUESTIONING_MSG "You have an ethical usecase for this, yes?"
#define GENERIC_MANUAL_TILER_MSG                                               \
  "I'm sure manual tiling saves you soooo much time."
#define GENERIC_XWM_MSG "Wayland is the future."

struct joke {
  char name[255];
  char comment[255];
};

extern bool teasing_enabled;
extern const struct joke known_desktops[NUM_OF_KNOWN_DESKTOPS];
extern const struct joke known_distros[NUM_OF_KNOWN_DISTROS];
extern const struct joke known_programs[NUM_OF_KNOWN_PROGRAMS];
extern const struct joke known_terminals[NUM_OF_KNOWN_TERMINALS];

void handle_teasing(struct repl_ctx *current_ctx);
bool program_is_blacklisted(const char *program_name);
void joke_binary_search(const struct joke *known_x, unsigned int array_size,
                        const char *target);
void tease_roll(struct repl_ctx *current_ctx, unsigned int command_index);
void tease_desktop(void);
void tease_distro(void);
void tease_kernel(void);
void tease_program(const char *program_name);
void tease_terminal(void);

#endif

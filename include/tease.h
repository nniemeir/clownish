/**
 * tease.h
 *
 * Interface for shell's teasing functionality, which can be disabled by using
 * the -p (polite) flag.
 */

#ifndef TEASE_H
#define TEASE_H

#include "stdbool.h"

#include "context.h"

/**
 * Array sizes for joke databases
 *
 * These MUST match the actual array sizes in tease_jokes.c
 */
#define NUM_OF_BLACKLISTED_PROGRAMS 3
#define NUM_OF_KNOWN_DESKTOPS 58
#define NUM_OF_KNOWN_DISTROS 64
#define NUM_OF_KNOWN_PROGRAMS 43
#define NUM_OF_KNOWN_TERMINALS 2

/* Common message templates */
#define ETHICAL_QUESTIONING_MSG "You have an ethical usecase for this, yes?"
#define GENERIC_MANUAL_TILER_MSG                                               \
  "I'm sure manual tiling saves you soooo much time."
#define GENERIC_XWM_MSG "Wayland is the future."

/**
 * joke - Pairs an item name with a comment
 *
 * Used for all teasing databases, arrays must be sorted by name for binary
 * search.
 */
struct joke {
  char name[255];
  char comment[255];
};

/**
 * teasing_enabled - Global flag for teasing system
 *
 * This gets set to false when the -p (polite) flag is set
 */
extern bool teasing_enabled;

/* Joke databases (defined in tease_jokes.c) */
extern const struct joke known_desktops[NUM_OF_KNOWN_DESKTOPS];
extern const struct joke known_distros[NUM_OF_KNOWN_DISTROS];
extern const struct joke known_programs[NUM_OF_KNOWN_PROGRAMS];
extern const struct joke known_terminals[NUM_OF_KNOWN_TERMINALS];

/**
 * handle_teasing - Entry point for teasing system
 * @current_ctx: Shell context
 *
 * Decides whether to tease the user after command execution, uses a static
 * toggle to alternate between teasing and not teasing. In a pipeline with
 * multiple commands, we loop through each command but only tease about
 * alternating ones based on the toggle.
 */
void handle_teasing(struct repl_ctx *current_ctx);

/**
 * program_is_blacklisted - Check if program should be refused
 * @program_name: Name of program to check
 *
 * Certain programs are blacklisted by the shell for the user's own good.
 *
 * Linear search is fine here since the blacklisted_programs array is small.
 *
 * Return: true if blacklisted, false otherwise
 */
bool program_is_blacklisted(const char *program_name);

/**
 * joke_binary_search - Search for item in sorted joke array
 * @known_x: Array of joke structs
 * @array_size: Number of elements in the array
 * @target: Name to search array for
 *
 * Searches for target in sorted array and prints associated joke if found.
 * This is a classic binary search algorithm
 *
 */
void joke_binary_search(const struct joke *known_x, unsigned int array_size,
                        const char *target);

/**
 * tease_roll: Randomly select and execute a teasing action
 * @current_ctx: Shell context
 * @command_index: Which command in pipeline to potentially tease about
 *
 * Rolls a random number and selects a teasing category based on ranges, with
 * each category having a 20% chance of being selected.
 */
void tease_roll(struct repl_ctx *current_ctx, unsigned int command_index);

/**
 * tease_desktop - Tease user about desktop environment
 *
 * Reads $XDG_CURRENT_DESKTOP to determine desktop environment and tease it if
 * recognized.
 *
 * XDG_CURRENT_DESKTOP has been part of the Desktop Entry
 * Specification since version 1.2 (2017), but is not universally respected (See
 * Sway WM PR #4876).
 */
void tease_desktop(void);

/**
 * tease_distro - Tease user about Linux distribution
 *
 * Reads /etc/os-release to determine distribution, then tease about it.
 */
void tease_distro(void);

/**
 * tease_kernel - Tease user about kernel version
 *
 * Uses uname() syscall to get kernel information and teases based on it.
 */
void tease_kernel(void);

/**
 * tease_terminal - Tease user about terminal emulator
 *
 * Reads $TERM to identify terminal emulator and teases about it.
 *
 * $TERM isn't perfect for this, as many terminals use the same value for it.
 */
void tease_terminal(void);

#endif

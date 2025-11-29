/**
 * tease_targets.c
 *
 * System detection and teasing for specific targets.
 *
 * OVERVIEW:
 * This module detects various aspects of the user's system and teases them
 * about them accurately. Each of the aspects that won't change during runtime
 * has a static variable that insures the shell only teases about it once per
 * session.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "error.h"
#include "file.h"
#include "tease.h"

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
bool program_is_blacklisted(const char *program_name) {
  static const struct joke blacklisted_programs[NUM_OF_BLACKLISTED_PROGRAMS] = {
      {"emacs", "No, use vim."},
      {"neofetch", "Do we really need another minimalist cyberpunk anime "
                   "girl tiling WM rice?"},
      {"surf", "Use a real web browser."}};

  for (int i = 0; i < NUM_OF_BLACKLISTED_PROGRAMS; i++) {
    if (strcmp(program_name, blacklisted_programs[i].name) == 0) {
      printf("%s\n", blacklisted_programs[i].comment);
      return true;
    }
  }
  return false;
}

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
void tease_desktop(void) {
  /* Prevent teasing about desktop more than once per run */
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  char *desktop = getenv("XDG_CURRENT_DESKTOP");
  if (!desktop) {
    error_msg(env_fail_msg, false);
    return;
  }

  joke_binary_search(known_desktops, NUM_OF_KNOWN_DESKTOPS, desktop);
}

/**
 * tease_distro - Tease user about Linux distribution
 *
 * Reads /etc/os-release to determine distribution, then tease about it.
 */
void tease_distro(void) {
  /* Prevent teasing about distro more than once per run */
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  char *os_release = read_file("/etc/os-release");

  /* Skip past 'NAME="' to get to the actual name */
  os_release = os_release + 6;

  /* Next quotation marks end of name */
  char *second_quote = strstr(os_release, "\"");

  *second_quote = '\0';

  joke_binary_search(known_distros, NUM_OF_KNOWN_DISTROS, os_release);

  /* Restore original pointer before freeing */
  os_release = os_release - 6;

  free(os_release);
}

/**
 * tease_kernel - Tease user about kernel version
 *
 * Uses uname() syscall to get kernel information and teases based on it.
 */
void tease_kernel(void) {
  /* Prevent teasing about kernel more than once per run */
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  struct utsname buffer;

  /* Fills buffer with system information */
  if (uname(&buffer) == -1) {
    error_msg("Failed to retrieve kernel information", true);
  }

  /* Check if user running Long-Term Support kernel */
  char *is_lts = strstr(buffer.release, "lts");

  if (is_lts) {
    printf("LTS? The future is now, old man.");
  }

  /* Check if kernel is zen variant */
  if (strstr(buffer.release, "zen")) {
    printf("You're on Zen? How's that placebo hitting for you?");
  }

  /* Check kernel major version, atoi truncates the string to the first number
   */
  int kernel_big_num = atoi(&buffer.release[0]);

  if (kernel_big_num < 6 && !is_lts) {
    printf("Please tell me this thing isn't connected to the internet on such "
           "an old kernel.\n");
  }
}

/**
 * tease_terminal - Tease user about terminal emulator
 *
 * Reads $TERM to identify terminal emulator and teases about it.
 *
 * $TERM isn't perfect for this, as many terminals use the same value for it.
 */
void tease_terminal(void) {
  /* Prevent teasing about terminal emulator more than once per run */
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  const char *term = getenv("TERM");
  if (!term) {
    error_msg(env_fail_msg, false);
  }

  /* Once more, linear search is fine for small arrays */
  for (int i = 0; i < NUM_OF_KNOWN_TERMINALS; i++) {
    if (strcmp(term, known_terminals[i].name) == 0) {
      printf("%s\n", known_terminals[i].comment);
      return;
    }
  }
}

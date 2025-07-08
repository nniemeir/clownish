#include "error.h"
#include "file.h"
#include "tease.h"

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

// XDG_CURRENT_DESKTOP has been part of the Desktop Entry Specification since
// version 1.2 (2017), but is not universally respected (See Sway WM PR #4876)
void tease_desktop(void) {
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

// os-release NAME values sourced from which-distro's collection
void tease_distro(void) {
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  char *os_release = read_file("/etc/os-release");

  os_release = os_release + 6;

  char *second_quote = strstr(os_release, "\"");

  *second_quote = '\0';

  joke_binary_search(known_distros, NUM_OF_KNOWN_DISTROS, os_release);

  os_release = os_release - 6;

  free(os_release);
}

void tease_kernel(void) {
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  struct utsname buffer;

  if (uname(&buffer) == -1) {
    error_msg("Failed to retrieve kernel information", true);
  }

  char *is_lts = strstr(buffer.release, "lts");

  if (is_lts) {
    printf("LTS? The future is now, old man.");
  }

  if (strstr(buffer.release, "zen")) {
    printf("You're on Zen? How's that placebo hitting for you?");
  }

  int kernel_big_num = atoi(&buffer.release[0]);

  if (kernel_big_num < 6 && !is_lts) {
    printf("Please tell me this thing isn't connected to the internet on such "
           "an old kernel.\n");
  }
}

// This will likely be overhauled in the future, as many terminals use the same
// TERM value
void tease_terminal(void) {
  static int called = 0;

  if (called == 1) {
    return;
  }

  called = 1;

  const char *term = getenv("TERM");
  if (!term) {
    error_msg(env_fail_msg, false);
  }

  for (int i = 0; i < NUM_OF_KNOWN_TERMINALS; i++) {
    if (strcmp(term, known_terminals[i].name) == 0) {
      printf("%s\n", known_terminals[i].comment);
      return;
    }
  }
}

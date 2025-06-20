#include "../include/tease.h"

int teasing_enabled = 1;

void tease_terminal(void) {
  const char *term = getenv("TERM");
  if (!term) {
    fprintf(stderr, "clowniSH: Unable to resolve TERM.\n");
  }

  static const struct joke known_terminals[NUM_OF_KNOWN_TERMINALS] = {
      {"st-256color", "May I be the first to say you're a very smart cookie."},
      {"xterm-kitty", "AN UPDATE FOR KITTY IS AVAILABLE."}};

  for (int i = 0; i < NUM_OF_KNOWN_TERMINALS; i++) {
    if (strcmp(term, known_terminals[i].name) == 0) {
      printf("%s\n", known_terminals[i].comment);
      return;
    }
  }
}

int program_is_blacklisted(const char *program_name) {
  static const struct joke blacklisted_programs[NUM_OF_BLACKLISTED_PROGRAMS] = {
      {"emacs", "No, use vim."}, {"surf", "Use a real web browser."}};

  for (int i = 0; i < NUM_OF_BLACKLISTED_PROGRAMS; i++) {
    if (strcmp(program_name, blacklisted_programs[i].name) == 0) {
      printf("%s\n", blacklisted_programs[i].comment);
      return 1;
    }
  }
  return 0;
}

void tease_program(const char *program_name) {
  static const struct joke known_programs[NUM_OF_KNOWN_PROGRAMS] = {
      {"bleachbit", "Pray that your email server isn't being audited."},
      {"code", "EEE, MICROSOFT BAD."},
      {"discord", "Make sure to enable application privacy before doing any independent research."},
      {"firefox", "The ship is sinking my friend."},
      {"gimp", "Do you miss Adobe yet?"},
      {"handbrake", "I hope this is all legitimately acquired media."},
      {"nvim", "Did Brodie or the Primeagen send you?"},
      {"ranger", "Ah I see, commands are just too hard for you."},
      {"steam", "Your To-Do list isn't getting any smaller"},
      {"thunar", "Are you too good for the CLI now?"},
      {"wireshark",
       "Why bother? You don't understand any of the packet fields anyway."}};

  unsigned int lower_bound = 0;
  unsigned int upper_bound = NUM_OF_KNOWN_PROGRAMS - 1;
  unsigned int match_found = 0;
  int middle_value;
  while (!match_found) {
    if (lower_bound > upper_bound) {
      middle_value = -1;
      break;
    }

    middle_value = (lower_bound + upper_bound) / 2;
    int comparison_result =
        strcmp(known_programs[middle_value].name, program_name);

    if (comparison_result < 0) {
      lower_bound = middle_value + 1;
      continue;
    }

    if (comparison_result == 0) {
      match_found = 1;
      break;
    }

    if (comparison_result > 0) {
      upper_bound = middle_value - 1;
      continue;
    }
  }

  if (match_found) {
    printf("%s\n", known_programs[middle_value].comment);
  }
}
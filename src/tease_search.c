#include <stdio.h>
#include <string.h>

#include "tease.h"

void joke_binary_search(const struct joke *known_x, unsigned int array_size,
                        const char *target) {
  unsigned int lower_bound = 0;
  unsigned int upper_bound = array_size - 1;
  unsigned int match_found = 0;
  int middle_value;

  while (!match_found) {
    if (lower_bound > upper_bound) {
      middle_value = -1;
      break;
    }

    middle_value = (lower_bound + upper_bound) / 2;

    int comparison_result = strcmp(known_x[middle_value].name, target);

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
    printf("%s\n", known_x[middle_value].comment);
  }
}

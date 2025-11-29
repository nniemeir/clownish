/**
 * tease_search.c
 *
 * Binary search for teasing database lookups.
 *
 * OVERVIEW:
 * Implements binary search to quickly find jokes about programs, distros,
 * desktops, etc. in sorted arrays.
 */

#include <stdio.h>
#include <string.h>

#include "tease.h"

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
                        const char *target) {
  unsigned int lower_bound = 0;
  unsigned int upper_bound = array_size - 1;
  unsigned int match_found = 0;
  int middle_value;

  while (!match_found) {
    /* If the range is empty, we've exhausted all possibilities */
    if (lower_bound > upper_bound) {
      middle_value = -1;
      break;
    }

    /**
     * Calculate the middle index, integer division will automatically truncate
     * in C: 9/2 = 4).
     */
    middle_value = (lower_bound + upper_bound) / 2;

    /* Compare target with middle element */
    int comparison_result = strcmp(known_x[middle_value].name, target);

    if (comparison_result < 0) {
      /**
       * Middle element comes before target alphabetically, so the target must be
       * in the right half if it is present.
       */
      lower_bound = middle_value + 1;
      continue;
    }

    if (comparison_result == 0) {
      match_found = 1;
      break;
    }

    if (comparison_result > 0) {
      /**
       * Middle element comes after target alphabetically, so the target msut be
       * in the left half if it is present.
       */
      upper_bound = middle_value - 1;
      continue;
    }
  }

  if (match_found) {
    printf("%s\n", known_x[middle_value].comment);
  }
}

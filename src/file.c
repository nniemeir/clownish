/**
 * file.c
 *
 * File I/O utility functions.
 *
 * OVERVIEW:
 * Provides helper functiosn for file operations. Used primarily for loading the
 * configuration file (~/.clownrc) at startup.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "error.h"
#include "file.h"
#include "parse.h"

/**
 * file_exists - Check if file exists
 * @filename: Path to file
 *
 * Uses stat() to check file existence. stat() returns 0 if the file exists and
 * is accessible, so we don't even need to check the contents of the buffer that
 * it fills with metadata to confirm this.
 *
 * Return: 1 if file exists, 0 otherwise
 */
int file_exists(const char *filename) {
  struct stat buffer;
  return stat(filename, &buffer) == 0 ? 1 : 0;
}

/**
 * read_file - Read entire file into memory
 * @file_path: Path to file to read
 *
 * Reads a file completely into a null-terminated string.
 *
 * Return: Allocated string with file contents, NULL on error
 */
char *read_file(const char *file_path) {
  /**
   * Though this is not a problem on Unix, I consider it good practice to
   * use binary read mode 'rb' for opening files because some operating systems
   * apply translations to certain characters in regular read mode 'r' which can
   * result in corruption during reading.
   */
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    error_msg(open_fail_msg, true);
    return NULL;
  }

  /* Move the file position indicator to end of file */
  if (fseek(file, 0, SEEK_END) == -1) {
    error_msg("Failed to set file position indicator", true);
    fclose(file);
    return NULL;
  }

  /* Get the current file position. Since we just moved to the end, this gives
   * us the file size*/
  long size = ftell(file);
  if (size == -1) {
    error_msg("Failed to get file position indicator", true);
    fclose(file);
    return NULL;
  }

  size_t file_size = (size_t)size;

  /* Move the position indicator back to the beginning */
  rewind(file);

  char *buffer = malloc(file_size + NULL_TERMINATOR_LENGTH);
  if (!buffer) {
    error_msg(malloc_fail_msg, true);
    fclose(file);
    return NULL;
  }

  const size_t bytes_read = fread(buffer, 1, file_size, file);

  /**
   * Partial read indicates an error in reading or that the file changed size
   * during read, we consider both to be error conditions.
   */
  if (bytes_read != file_size) {
    error_msg("Failed to read from stream", true);
    free(buffer);
    fclose(file);
    return NULL;
  }

  fclose(file);

  /* Null-terminate the buffer so that we can use it as a string */
  buffer[file_size] = '\0';

  return buffer;
}

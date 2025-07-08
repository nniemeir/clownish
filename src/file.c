#include "file.h"
#include "error.h"
#include "parse.h"

int file_exists(const char *filename) {
  struct stat buffer;
  return stat(filename, &buffer) == 0 ? 1 : 0;
}

char *read_file(const char *file_path) {
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    error_msg(open_fail_msg, true);
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    error_msg("Failed to set file position indicator", true);
    fclose(file);
    return NULL;
  }

  long size = ftell(file);
  if (size == -1) {
    error_msg("Failed to get file position indicator", true);
    fclose(file);
    return NULL;
  }

  size_t file_size = (size_t)size;

  rewind(file);

  char *buffer = malloc(file_size + NULL_TERMINATOR_LENGTH);
  if (!buffer) {
    error_msg(malloc_fail_msg, true);
    fclose(file);
    return NULL;
  }

  const size_t bytes_read = fread(buffer, 1, file_size, file);

  if (bytes_read != file_size) {
    error_msg("Failed to read from stream", true);
    free(buffer);
    fclose(file);
    return NULL;
  }

  fclose(file);

  buffer[file_size] = '\0';

  return buffer;
}

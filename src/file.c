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
    fprintf(stderr, "Failed to open %s", file_path);
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    fprintf(stderr, "fseek failed.");
    fclose(file);
    return NULL;
  }

  long size = ftell(file);
  if (size == -1) {
    fprintf(stderr, "ftell failed.");
    fclose(file);
    return NULL;
  }
  size_t file_size = (size_t)size;

  rewind(file);

  char *buffer;
  buffer = malloc(file_size + NULL_TERMINATOR_LENGTH);
  if (!buffer) {
    fprintf(stderr, malloc_fail_msg, "file buffer");
    fclose(file);
    return NULL;
  }
  const size_t bytes_read = fread(buffer, 1, file_size, file);
  if (bytes_read != file_size) {
    fprintf(stderr, "fread failed.");
    free(buffer);
    fclose(file);
    return NULL;
  }
  fclose(file);
  buffer[file_size] = '\0';
  return buffer;
}
#include "file.h"

char *read_file(const char *file_path) {
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    // log_event(program_name, ERROR,
             // "read_file failed to open the requested file.", // log_to_file);
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    // log_event(program_name, ERROR, "fseek failed.", // log_to_file);
    fclose(file);
    return NULL;
  }

  long size = ftell(file);
  if (size == -1) {
    // log_event(program_name, ERROR, "ftell failed.", // log_to_file);
    fclose(file);
    return NULL;
  }
  size_t file_size = (size_t)size;

  rewind(file);

  char *buffer;
  buffer = malloc(file_size);
  if (!buffer) {
    // log_event(program_name, ERROR, "Failed to allocate memory for file buffer.",
              // log_to_file);
    fclose(file);
    return NULL;
  }
  const size_t bytes_read = fread(buffer, 1, file_size, file);
  if (bytes_read != file_size) {
    // log_event(program_name, ERROR, "Error reading file into buffer.",
              // log_to_file);
    free(buffer);
    fclose(file);
    return NULL;
  }
  fclose(file);
  return buffer;
}
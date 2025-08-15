#include "history.h"
#include "error.h"
#include "file.h"

char *init_history(char *home_dir) {
  char *hist_file = malloc(PATH_MAX);
  if (!hist_file) {
    error_msg(malloc_fail_msg, true);
    return NULL;
  }

  snprintf(hist_file, PATH_MAX, "%s/.clown_scribbles", home_dir);

  using_history();

  if (!file_exists(hist_file)) {
    return hist_file;
  }

  if (read_history(hist_file) != 0) {
    error_msg("Failed to read history", false);
    free(hist_file);
    return NULL;
  }

  return hist_file;
}

void close_history(char *hist_file) {
  if (write_history(hist_file) != 0) {
    error_msg("Failed to write history", false);
  }

  free(hist_file);

  clear_history();
}

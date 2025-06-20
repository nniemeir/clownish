#include "../include/history.h"

char *init_history(char *home_dir) {
  char *hist_file = malloc(PATH_MAX);
  snprintf(hist_file, PATH_MAX, "%s/.scribbles", home_dir);
  using_history();
  read_history(hist_file);
  return hist_file;
}

void close_history(char *hist_file) {
  write_history(hist_file);
  free(hist_file);
  clear_history();
}
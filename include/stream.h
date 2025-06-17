#ifndef STREAM_H
#define STREAM_H
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct repl_ctx {
  char *home_dir;
  char *input;
  char *parsed_str;
  char **args;
  unsigned int args_count;
  int is_background_process;
  char in_stream_name[256];
  int in_stream_type;
  char out_stream_name[256];
  int out_stream_type;
};

int determine_out_stream(struct repl_ctx *current_ctx);

#endif
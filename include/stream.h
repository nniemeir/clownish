#ifndef STREAM_H
#define STREAM_H
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct stream_info {
  char name[256];
  int type;
};

int determine_stream(char *input, struct stream_info *current_stream_info);

#endif
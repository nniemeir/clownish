#include "../include/stream.h"

int determine_stream(char *input, struct stream_info *current_stream_info) {
  current_stream_info->type = 0;
  strcpy(current_stream_info->name, "");
  char *gt = strstr(input, " > ");
  if (gt) {
    current_stream_info->type = O_WRONLY;
    gt = gt + 3;
    strcpy(current_stream_info->name, gt);
    gt = gt - 3;
    *gt = '\0';
    return 1;
  }
  char *two_gt = strstr(input, " >> ");
  if (two_gt) {
    current_stream_info->type = O_APPEND;
    two_gt = two_gt + 4;
    strcpy(current_stream_info->name, two_gt);
    two_gt = two_gt - 4;
    *two_gt = '\0';
  }
  return 1;
}

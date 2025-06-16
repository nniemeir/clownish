#include "../include/stream.h"

int determine_stream(struct repl_ctx *current_ctx) {
  current_ctx->stream_type = 0;
  strcpy(current_ctx->stream_name, "");
  char *gt = strstr(current_ctx->input, " > ");
  if (gt) {
    current_ctx->stream_type = O_WRONLY;
    gt = gt + 3;
    strcpy(current_ctx->stream_name, gt);
    gt = gt - 3;
    *gt = '\0';
    return 1;
  }
  char *two_gt = strstr(current_ctx->input, " >> ");
  if (two_gt) {
    current_ctx->stream_type = O_APPEND;
    two_gt = two_gt + 4;
    strcpy(current_ctx->stream_name, two_gt);
    two_gt = two_gt - 4;
    *two_gt = '\0';
  }
  return 1;
}

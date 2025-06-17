#include "../include/stream.h"

int determine_out_stream(struct repl_ctx *current_ctx) {
  current_ctx->out_stream_type = 0;
  strcpy(current_ctx->out_stream_name, "");
  char *gt = strstr(current_ctx->input, " > ");
  if (gt) {
    current_ctx->out_stream_type = O_WRONLY;
    gt = gt + 3;
    strcpy(current_ctx->out_stream_name, gt);
    gt = gt - 3;
    *gt = '\0';
    return 1;
  }
  char *two_gt = strstr(current_ctx->input, " >> ");
  if (two_gt) {
    current_ctx->out_stream_type = O_APPEND;
    two_gt = two_gt + 4;
    strcpy(current_ctx->out_stream_name, two_gt);
    two_gt = two_gt - 4;
    *two_gt = '\0';
  }
  return 1;
}
/*
int determine_in_stream(struct repl_ctx *current_ctx) {
  strcpy(current_ctx->in_stream_name, "");
  char *lt = strstr(current_ctx->input, " >");
  if (lt) {
    current_ctx->out_stream_type = O_WRONLY;
    lt = lt + 2;
    strcpy(current_ctx->out_stream_name, lt);
    lt = lt - 2;
    *lt = '\0';
    return 1;
  }
}*/
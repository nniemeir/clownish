#include "main.h"
#include "config.h"
#include "context.h"
#include "error.h"
#include "exec.h"
#include "history.h"
#include "input.h"
#include "signals.h"
#include "tease.h"

void process_args(int argc, char *argv[]) {
  int c;
  while ((c = getopt(argc, argv, "dhpv")) != -1) {
    switch (c) {
    case 'd':
      debug_mode = true;
      break;
    case 'h':
      printf("Usage: clownish [options]\n");
      printf("Options:\n");
      printf("  -d               enable debug mode\n");
      printf("  -h               Show this help message\n");
      printf("  -p               Enable polite mode\n");
      printf("  -v               Show version info\n");
      exit(EXIT_SUCCESS);
    case 'p':
      teasing_enabled = false;
      break;
    case 'v':
      printf("clowniSH 0.231969420: Malevolent Marlin\n");
      exit(EXIT_SUCCESS);
    case '?':
      fprintf(stderr, "Unknown option '-%c'. Run with -h for options.\n",
              optopt);
      exit(EXIT_FAILURE);
    }
  }
}

bool skip_execution(struct repl_ctx *current_ctx) {
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    if (program_is_blacklisted(current_ctx->commands[i][0])) {
      return true;
    }
  }
  return false;
}

void repl(struct repl_ctx *current_ctx, char *hist_file) {
  while (current_ctx->receiving) {
    if (take_input(current_ctx) == 1) {
      cleanup_ctx(current_ctx);
      close_history(hist_file);
      exit(EXIT_FAILURE);
    }

    if (current_ctx->input[0] == '\0') {
      cleanup_ctx(current_ctx);
      continue;
    }

    if (process_input(current_ctx) == 1) {
      cleanup_ctx(current_ctx);
      close_history(hist_file);
      exit(EXIT_FAILURE);
    }

    if (skip_execution(current_ctx)) {
      cleanup_ctx(current_ctx);
      continue;
    }

    if (exec(current_ctx) == 1) {
      cleanup_ctx(current_ctx);
      continue;
    }

    handle_teasing(current_ctx);

    cleanup_ctx(current_ctx);
  }
}

int main(int argc, char *argv[]) {
  if (geteuid() == 0) {
    error_msg("You attempted to run this shell as root, I cannot sanction your "
              "buffoonery.\n",
              false);
    exit(EXIT_FAILURE);
  }

  process_args(argc, argv);

  struct repl_ctx current_ctx;

  init_current_ctx(&current_ctx);

  char *hist_file = init_history(current_ctx.home_dir);

  if (!hist_file) {
    exit(EXIT_FAILURE);
  }

  if (init_sig_handler() == 1) {
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  current_ctx.receiving = 1;
  current_ctx.input = NULL;

  repl(&current_ctx, hist_file);

  close_history(hist_file);

  exit(EXIT_SUCCESS);
}

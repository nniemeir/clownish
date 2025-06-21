#include "../include/main.h"
#include "../include/envs.h"
#include "../include/exec.h"
#include "../include/history.h"
#include "../include/parse.h"
#include "../include/prompt.h"
#include "../include/tease.h"

void handler(int signal_num) { write(STDOUT_FILENO, "\n", 2); }

void process_args(int argc, char *argv[]) {
  int c;
  while ((c = getopt(argc, argv, "hpv")) != -1) {
    switch (c) {
    case 'h':
      printf("Usage: clownish [options]\n");
      printf("Options:\n");
      printf("  -h               Show this help message\n");
      printf("  -p               Enable polite mode\n");
      printf("  -v               Show version info\n");
      exit(EXIT_SUCCESS);
    case 'p':
      teasing_enabled = 0;
      break;
    case 'v':
      printf("clowniSH Pre-Alpha\n");
      exit(EXIT_SUCCESS);
    case '?':
      fprintf(stderr, "Unknown option '-%c'. Run with -h for options.\n",
              optopt);
      exit(EXIT_FAILURE);
    }
  }
}

void cleanup_ctx(struct repl_ctx *current_ctx) {
  // Allocation of args only occurs if the string is not empty
  if (current_ctx->input[0] != '\0') {
    free(current_ctx->args);
  }
  if (current_ctx->input) {
    free(current_ctx->input);
  }
}

int main(int argc, char *argv[]) {
  process_args(argc, argv);

  if (teasing_enabled) {
    tease_kernel();
    tease_terminal();
  }

  struct repl_ctx current_ctx;
  current_ctx.home_dir = init_home_dir();
  if (!current_ctx.home_dir) {
    exit(EXIT_FAILURE);
  }

  current_ctx.user = init_user();

  char *hist_file = init_history(current_ctx.home_dir);
  if (!hist_file) {
    exit(EXIT_FAILURE);
  }

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  sigaction(SIGINT, &sa, NULL);

  srand(time(NULL));

  current_ctx.receiving = 1;
  current_ctx.input = NULL;
  while (current_ctx.receiving) {
    if (prompt_loop(&current_ctx)) {
      cleanup_ctx(&current_ctx);
      close_history(hist_file);
      exit(EXIT_FAILURE);
    }

    if (current_ctx.input[0] == '\0') {
      cleanup_ctx(&current_ctx);
      continue;
    }

    if (program_is_blacklisted(current_ctx.args[0])) {
      cleanup_ctx(&current_ctx);
      continue;
    }

    if (exec(&current_ctx) == 1) {
      printf("clowniSH: Failed to execute command.\n");
      cleanup_ctx(&current_ctx);
      continue;
    }

    if (teasing_enabled) {
      tease_program(current_ctx.args[0]);
    }

    cleanup_ctx(&current_ctx);
  }
  close_history(hist_file);
  exit(EXIT_SUCCESS);
}
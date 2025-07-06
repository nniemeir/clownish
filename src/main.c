#include "main.h"
#include "config.h"
#include "envs.h"
#include "error.h"
#include "exec.h"
#include "file.h"
#include "history.h"
#include "parse.h"
#include "prompt.h"
#include "tease.h"

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
      printf("clowniSH 0.231969420: Malevolent Marlin\n");
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
    for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
      if (current_ctx->commands[i]) {
        for (unsigned int j = 0; j < current_ctx->args_count[i]; j++) {
          free(current_ctx->commands[i][j]);
        }
        free(current_ctx->commands[i]);
      }
      if (current_ctx->unparsed_commands[i]) {
        free(current_ctx->unparsed_commands[i]);
      }
      if (current_ctx->in_stream_name[i]) {
        free(current_ctx->in_stream_name[i]);
      }
      if (current_ctx->out_stream_name[i]) {
        free(current_ctx->out_stream_name[i]);
      }
    }
    if (current_ctx->out_stream_name) {
      free(current_ctx->out_stream_name);
    }
    if (current_ctx->out_stream_type) {
      free(current_ctx->out_stream_type);
    }
    if (current_ctx->args_count) {
      free(current_ctx->args_count);
    }
    if (current_ctx->commands) {
      free(current_ctx->commands);
    }
    if (current_ctx->in_stream_name) {
      free(current_ctx->in_stream_name);
    }
    if (current_ctx->in_stream_type) {
      free(current_ctx->in_stream_type);
    }
    if (current_ctx->unparsed_commands) {
      free(current_ctx->unparsed_commands);
    }
  }
  if (current_ctx->input) {
    free(current_ctx->input);
  }
}

int init_sig_handler() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror(program_name);
    return 1;
  }
  return 0;
}

void init_current_ctx(struct repl_ctx *current_ctx) {
  current_ctx->home_dir = init_home_dir();
  if (!current_ctx->home_dir) {
    exit(EXIT_FAILURE);
  }

  load_config(current_ctx);

  current_ctx->user = getenv_checked("USER", "Keith");
}

int main(int argc, char *argv[]) {
  if (geteuid() == 0) {
    fprintf(stderr,
            "Running this as root? I cannot sanction your buffoonery.\n");
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
  bool teasing_current_command = true;
  while (current_ctx.receiving) {
    if (prompt_loop(&current_ctx) == 1) {
      cleanup_ctx(&current_ctx);
      close_history(hist_file);
      exit(EXIT_FAILURE);
    }

    if (current_ctx.input[0] == '\0') {
      cleanup_ctx(&current_ctx);
      continue;
    }

    for (unsigned int i = 0; i < current_ctx.commands_count; i++) {
      if (program_is_blacklisted(current_ctx.commands[i][0])) {
        cleanup_ctx(&current_ctx);
        continue;
      }
    }
    if (exec(&current_ctx) == 1) {
      cleanup_ctx(&current_ctx);
      continue;
    }
    for (unsigned int j = 0; j < current_ctx.commands_count; j++) {
      if (teasing_enabled && current_ctx.commands[j][0][0] != '\0') {
        if (teasing_current_command) {
          tease_roll(&current_ctx, j);
        }
        teasing_current_command = !teasing_current_command;
      }
    }

    cleanup_ctx(&current_ctx);
  }
  close_history(hist_file);
  exit(EXIT_SUCCESS);
}
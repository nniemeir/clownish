#include "../include/main.h"
#include "../include/exec.h"
#include "../include/history.h"
#include "../include/prompt.h"
#include "../include/stream.h"

void process_args(int argc, char *argv[]) {
  int c;
  while ((c = getopt(argc, argv, "hv")) != -1) {
    switch (c) {
    case 'h':
      printf("Usage: clownish [options]\n");
      printf("Options:\n");
      printf("  -h               Show this help message\n");
      printf("  -v               Show version info\n");
      exit(EXIT_SUCCESS);
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

int main(int argc, char *argv[]) {
  process_args(argc, argv);
  char *hist_file = init_history();
  if (!hist_file) {
    exit(EXIT_FAILURE);
  }
  int receiving = 1;
  char *input = NULL;
  char **args;
  while (receiving) {
    struct stream_info *current_stream_info =
        malloc(sizeof(struct stream_info));
    int args_count = 0;
    int is_background_process = 0;
    if (prompt_loop(&args, &input, &args_count, &is_background_process,
                    current_stream_info)) {
      free(hist_file);
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    }
    if (input[0] == '\0') {
      free(input);
      continue;
    }
    int command_is_builtin =
        exec_builtin(args, &receiving);
    switch (command_is_builtin) {
    case -1:
      printf("clowniSH: Failed to execute built-in command.\n");
      free(hist_file);
      free(current_stream_info);
      free(input);
      free(args);
      exit(EXIT_FAILURE);
    case 1:
      free(input);
      free(current_stream_info);
      free(args);
      continue;
    }
    if (exec(args, current_stream_info, is_background_process) == 1) {
      printf("clowniSH: Failed to execute command.\n");
      free(hist_file);
      free(current_stream_info);
      free(input);
      free(args);
      continue;
    }
    free(input);
    free(args);
    free(current_stream_info);
  }
  close_history(hist_file);
  exit(EXIT_SUCCESS);
}
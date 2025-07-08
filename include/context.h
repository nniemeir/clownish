#ifndef CONTEXT_H
#define CONTEXT_H

struct user_env {
  char *name;
  char *value;
};

struct repl_ctx {
  char *home_dir;
  char *config_filename;
  struct user_env *user_envs;
  unsigned int user_envs_count;
  char *user;
  int receiving;
  char *input;
  char ***commands;
  char **unparsed_commands;
  unsigned int commands_count;
  unsigned int *args_count;
  int is_background_process;
  char **in_stream_name;
  int *in_stream_type;
  char **out_stream_name;
  int *out_stream_type;
};

void cleanup_ctx(struct repl_ctx *current_ctx);
void init_current_ctx(struct repl_ctx *current_ctx);

#endif

#include "config.h"
#include "error.h"
#include "parse.h"

void parse_envs(char **arg, struct user_env *user_envs,
                unsigned int user_envs_count) {
  char *start = *arg;
  if (strstr(start, "$")) {
    char var_name[ENV_MAX];
    unsigned int i = 0;
    start++;
    while (*start && (isalnum(*start) || *start == '_')) {
      var_name[i++] = *start++;
    }
    var_name[i] = '\0';
    char *env_value;
    if (user_envs_count > 0) {
      env_value = get_user_env(var_name, user_envs, user_envs_count);
      if (env_value) {
        *arg = env_value;
        return;
      }
    }
    env_value = getenv(var_name);
    if (!env_value) {
      error_msg(env_fail_msg, false);
      env_value = "";
      return;
    }

    *arg = strdup(env_value);
    if (!*arg) {
      error_msg(strdup_fail_msg, true);
    }
    return;
  }
}
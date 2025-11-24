#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

#define ERR_MSG_MAX 2048

extern const char *program_name;
extern const char *blame_user_msg;
extern const char *close_fail_msg;
extern const char *dup2_fail_msg;
extern const char *env_fail_msg;
extern const char *malloc_fail_msg;
extern const char *open_fail_msg;
extern const char *redirection_missing_filename_msg;
extern const char *strdup_fail_msg;

void error_msg(const char *function_name, bool report_errno);

#endif 

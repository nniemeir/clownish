#ifndef HISTORY_H
#define HISTORY_H
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/history.h>

char *init_history();
void close_history(char *hist_file);
#endif
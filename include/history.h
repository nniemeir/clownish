#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <readline/history.h>

char *init_history(char *home_dir);
void close_history(char *hist_file);

#endif
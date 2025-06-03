#ifndef PARSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIM " \t\r\n\a"

char **split_input(char *line, int *args_count);
#endif
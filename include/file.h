#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int file_exists(const char *filename);
char *read_file(const char *file_path);

#endif

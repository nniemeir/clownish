/**
 * file.h
 *
 * Provides helper functions for common file operations needed by the shell,
 * primarily for loading configuration files.
 */

#ifndef FILE_H
#define FILE_H

/**
 * file_exists - Check if file exists
 * @filename: Path to file
 *
 * Uses stat() to check file existence. stat() returns 0 if the file exists and
 * is accessible, so we don't even need to check the contents of the buffer that
 * it fills with metadata to confirm this.
 *
 * Return: 1 if file exists, 0 otherwise
 */
int file_exists(const char *filename);

/**
 * read_file - Read entire file into memory
 * @file_path: Path to file to read
 *
 * Reads a file completely into a null-terminated string.
 *
 * Return: Allocated string with file contents, NULL on error
 */
char *read_file(const char *file_path);

#endif

/***********************************************************************
 *  File name   : validate.h
 *  Description : Header file for validation-related functions in the
 *                Inverted Search Project.
 *                Provides prototypes for validating:
 *                - File names and file sizes
 *                - Command-line arguments (file list)
 *                - Word hashing index
 *                - Existing database file format
 *
 *                Functions:
 *                - get_file_size()
 *                - read_and_validate_args()
 *                - get_word_index()
 *                - valid_file_name()
 *                - valid_database()
 *
 ***********************************************************************/

#ifndef VALIDATE_H
#define VALIDATE_H

#include "list.h"

/**
 * Returns the size of the given file (in bytes).
 */
size_t get_file_size(FILE *fp);

/**
 * Reads command-line arguments, validates files, 
 * and builds the FileList linked list.
 * Returns SUCCESS or FAILURE.
 */
int read_and_validate_args(FileList **filelist, char **argv, int argc);

/**
 * Returns the hash bucket index for a given word based on its first character.
 * A-Z or a-z → 0-25, digits → 26, other → 27.
 * Returns -1 for NULL or empty input.
 */
int get_word_index(char *word);

/**
 * Validates whether a given filename is acceptable
 * (i.e., correct extension, not empty, within limits).
 */
int valid_file_name(char *filename);

/**
 * Validates an existing database file format before loading/updating.
 */
int valid_database(FILE *fp);


#endif

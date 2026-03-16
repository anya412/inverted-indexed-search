/***********************************************************************
 *  File name   : database.h
 *  Description : Header file for database operations in the Inverted Search project.
 *                Provides function prototypes for creating, displaying, 
 *                searching, saving, and updating the database.
 ***********************************************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include "list.h"
#include "validate.h"

/* Create the database (hash table) from given file list */
int create_database(FileList *filelist, HashTable hashTable[MAX_HASH_SIZE]);

/* Display the contents of the database */
void display_database(HashTable hashTable[]);

/* Search for a word in the database */
void search_word(HashTable hashTable[], char *word);

/* Save the database to a backup file */
void save_database(HashTable hashTable[], char *backup);

/* Update the database with new files and save changes */
void update_database(FileList **filelist, HashTable hashTable[], char *backup);

#endif

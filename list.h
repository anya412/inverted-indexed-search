/***********************************************************************
 *  File name   : list.h
 *  Description : Header file for linked list and hash table structures 
 *                used in the Inverted Indexed Search Project.
 *                Contains structure definitions and function prototypes 
 *                for managing:
 *                - File list (input files)
 *                - Main node (word entries)
 *                - Sub node (filename and word count mapping)
 *                - Hash table (inverted index)
 *
 *                Functions:
 *                - fileList_insert_last()
 *                - initialize_hashTable()
 *                - hashTable_insert_last()
 *                - create_mainNode()
 *                - create_subNode()
 *                - delete_duplicate()
 *                - print_fileList()
 ***********************************************************************/

#ifndef LIST_H
#define LIST_H

// Required Header Files
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Predefined Macros
#define MAX_FILENAME_LENGTH 20   // Maximum length of filename
#define MAX_WORD_LENGTH 20       // Maximum length of a word
#define MAX_HASH_SIZE 28         // Hash table size (A-Z + extra buckets)

#define SUCCESS 0
#define FAILURE -1
#define DUPLICATE -2
#define LIST_EMPTY -3

/* ----------- Structures ----------- */

/* SubNode:
 * Stores filename and the count of occurrences of a word in that file
 */
typedef struct SubNode
{
    char filename[MAX_FILENAME_LENGTH];
    int wordCount;
    struct SubNode *subLink;   // Pointer to next SubNode
} SubNode;

/* MainNode:
 * Stores a unique word, count of files it appears in,
 * and a linked list of SubNodes (file → wordCount mapping)
 */
typedef struct MainNode
{
    char word[MAX_WORD_LENGTH];
    int fileCount;
    struct SubNode *subLink;   // Linked list of files containing this word
    struct MainNode *mainLink; // Pointer to next MainNode
} MainNode;

/* FileList:
 * Singly linked list to store input filenames
 */
typedef struct FileList
{
    char filename[MAX_FILENAME_LENGTH];
    struct FileList *link;     // Pointer to next file in the list
} FileList;

/* HashTable:
 * Stores index (bucket) and pointer to MainNode linked list
 */
typedef struct HashTable
{
    int index;                 // Hash index (0-27)
    struct MainNode *link;     // Linked list of MainNodes at this index
} HashTable;

/* ----------- Function Prototypes ----------- */

// Inserts a filename at the end of FileList
int fileList_insert_last(FileList **filelist, char * filename);

// Initializes the hash table with NULL links
void initialize_hashTable(HashTable *hashTablle,int size);

// Inserts a word into hash table under a given index, along with filename
int hashTable_insert_last(HashTable hashTablle[MAX_HASH_SIZE], char *filename, int index, char *word);

// Creates a new MainNode for a word
MainNode *create_mainNode(char * word, int fileCount);

// Creates a new SubNode for a filename and wordCount
SubNode *create_subNode(char *filename,int wordCount);

// Deletes duplicate filenames from FileList
int delete_duplicate(FileList **filelist, char *filename);

// Prints the list of input files
void print_fileList(FileList *fileList);

#endif

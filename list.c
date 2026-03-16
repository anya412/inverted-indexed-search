/***********************************************************************
 *  File name   : list.c
 *  Description : Implementation file for linked list and hash table 
 *                operations in the Inverted Search Project.
 *                Provides functions for:
 *                - File list management
 *                - Hash table initialization and insertion
 *                - Node creation (MainNode, SubNode)
 *                - Duplicate removal
 *                - File list printing
 *
 *                Functions:
 *                - initialize_hashTable()
 *                - fileList_insert_last()
 *                - hashTable_insert_last()
 *                - create_mainNode()
 *                - create_subNode()
 *                - delete_duplicate()
 *                - print_fileList()
 ***********************************************************************/

#include "list.h"
#include "validate.h"

// Initializes hash table with indices and NULL links
void initialize_hashTable(HashTable *hashTablle, int size)
{
    for (int i = 0; i < size; i++)
    {
        hashTablle[i].index = i;
        hashTablle[i].link = NULL;
    }
}

/**
 * Inserts a filename at the end of FileList
 * Returns SUCCESS, FAILURE, or DUPLICATE
 */
int fileList_insert_last(FileList **filelist, char *filename)
{
    FileList *new = malloc(sizeof(FileList));
    if (new == NULL)
    {
        printf("File could not be created\n");
        return FAILURE;
    }

    strcpy(new->filename, filename);
    new->link = NULL;

    // If list is empty, insert first node
    if (*filelist == NULL)
    {
        *filelist = new;
        return SUCCESS;
    }

    // Traverse to end of list
    FileList *temp = *filelist;
    while (temp && temp->link)
    {
        // Check duplicate
        if (strcmp(temp->filename, filename) == 0)
            return DUPLICATE;
        temp = temp->link;
    }

    // Check duplicate for last node
    if (strcmp(temp->filename, filename) == 0)
        return DUPLICATE;

    temp->link = new;
    return SUCCESS;
}

/**
 * Inserts a word into hash table at a given index
 * Handles creation of MainNode (word) and SubNode (filename)
 */
int hashTable_insert_last(HashTable hashTablle[MAX_HASH_SIZE], char *filename, int index, char *word)
{
    if (index < 0 || index >= MAX_HASH_SIZE) 
        return FAILURE;

    MainNode *curr_m = hashTablle[index].link;
    MainNode *prev_m = NULL;

    // Traverse MainNode list for the given index
    while (curr_m)
    {
        if (strcmp(curr_m->word, word) == 0)
        {
            // Word exists → update SubNode list
            SubNode *curr_sub = curr_m->subLink;
            SubNode *prev_sub = NULL;

            while (curr_sub)
            {
                // If word already exists in this file → increment count
                if (strcmp(curr_sub->filename, filename) == 0)
                {
                    curr_sub->wordCount++;
                    return SUCCESS;
                }
                prev_sub = curr_sub;
                curr_sub = curr_sub->subLink;
            }

            // Word exists but file not found → create new SubNode
            SubNode *newSub = create_subNode(filename, 1);
            if (newSub == NULL)
                return FAILURE;

            if (prev_sub)
                prev_sub->subLink = newSub;
            else
                curr_m->subLink = newSub;

            curr_m->fileCount++;
            return SUCCESS;
        }
        prev_m = curr_m;
        curr_m = curr_m->mainLink;
    }

    // Word not found → create new MainNode with SubNode
    MainNode *newMain = create_mainNode(word, 1);
    if (newMain == NULL)
        return FAILURE;

    newMain->subLink = create_subNode(filename, 1);

    if (prev_m)
        prev_m->mainLink = newMain;
    else
        hashTablle[index].link = newMain;

    return SUCCESS;
}

// Creates a new MainNode for a given word
MainNode *create_mainNode(char *word, int fileCount)
{
    MainNode *newMain = malloc(sizeof(MainNode));
    if (newMain == NULL)
        return NULL;

    newMain->fileCount = fileCount;
    strcpy(newMain->word, word);
    newMain->mainLink = NULL;

    return newMain;
}

// Creates a new SubNode for a given filename and wordCount
SubNode *create_subNode(char *filename, int wordCount)
{
    SubNode *newSub = malloc(sizeof(SubNode));
    if (newSub == NULL)
        return NULL;

    strcpy(newSub->filename, filename);
    newSub->wordCount = wordCount;
    newSub->subLink = NULL;

    return newSub;
}

/**
 * Deletes a duplicate filename from FileList
 * Returns SUCCESS if deleted, FAILURE if not found
 */
int delete_duplicate(FileList **filelist, char *filename)
{
    FileList *curr = *filelist;
    FileList *prev = NULL;

    while (curr)
    {
        if (strcmp(curr->filename, filename) == 0)
        {
            if (prev == NULL)
            {
                // First node is duplicate
                FileList *del = *filelist;
                *filelist = del->link;
                free(del);
                return SUCCESS;
            }

            // Remove middle or last node
            prev->link = curr->link;
            free(curr);
            return SUCCESS;
        }

        prev = curr;
        curr = curr->link;
    }
    return FAILURE;
}

// Prints the list of input filenames
void print_fileList(FileList *fileList)
{
    printf("FileList: ");
    while (fileList)
    {
        printf("-> %s ", fileList->filename);
        fileList = fileList->link;
    }
    printf("\n");
}

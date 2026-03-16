/***********************************************************************
 *  File name   : database.c
 *  Description : Implementation file for database operations in the 
 *                Inverted Search project. Handles creating, displaying,
 *                searching, saving, and updating the database.
 ***********************************************************************/

#include "database.h"

/* Create database from input files and store words in hash table */
int create_database(FileList *filelist, HashTable hashTablle[MAX_HASH_SIZE])
{
    if(filelist == NULL)
    {
        fprintf(stderr, "\nINFO: File List is Empty\n");
        return FAILURE;
    }
    FileList *temp = filelist;
    while(temp)
    {
        FILE *fp = fopen(temp->filename, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Error: Could not open file '%s'\n", temp->filename);
            temp = temp->link;
            continue; // skip this file
        }
        char word[MAX_WORD_LENGTH];
        while(fscanf(fp, "%19s", word) == 1)
        {
            int index = get_word_index(word);
            if (index < 0 || index >= MAX_HASH_SIZE)
            {
                fprintf(stderr, "INFO: Skipping word '%s' (index %d out of range)\n", word, index);
                continue;
            }
            if (hashTable_insert_last(hashTablle, temp->filename, index, word) != SUCCESS)
                fprintf(stderr, "INFO: Failed to insert word %s from file %s\n", word, temp->filename);
        }
        fclose(fp);
        printf("\nINFO: DATABASE successfully created for file %s\n", temp->filename);
        temp = temp->link;
    }
    return SUCCESS;
}

/* Display the contents of the database in a table format */
void display_database(HashTable hashTablle[])
{
    printf("====================================================================================\n");
    printf("| %-10s%-15s%15s%20s%20s |\n", "Index", "Word", "File Count", "File Name", "word Count");
    for(int i = 0; i < MAX_HASH_SIZE; i++)
    {
        MainNode *temp = hashTablle[i].link;
        while(temp != NULL)
        {
            printf("|----------------------------------------------------------------------------------|\n");
            printf("| %-10d%-15s%15d", i, temp->word, temp->fileCount);
            SubNode *temp_sub = temp->subLink;
            for(int j = 0; j < temp->fileCount; j++)
            {
                if(j > 0)
                {
                    printf("|%-40s ", "           ->");
                }
                printf("%20s%20d |\n", temp_sub->filename, temp_sub->wordCount);
                temp_sub = temp_sub->subLink;
            }
            temp = temp->mainLink;
        }
    }
    printf("====================================================================================\n");
}

/* Search for a given word in the database */
void search_word(HashTable hashTablle[], char *word)
{
    int index = get_word_index(word);
    MainNode *temp_m = hashTablle[index].link;
    int found = 0;
    while(temp_m)
    {
        if(strcmp(temp_m->word, word) == 0)
        {
            found = 1;
            printf("\nWord '%s' is present in (%d) file\n", temp_m->word, temp_m->fileCount);
            SubNode *temp_s = temp_m->subLink;
            for(int j = 0; j < temp_m->fileCount; j++)
            {
                printf("In File : '%s' (%d) Time\n", temp_s->filename, temp_s->wordCount);
                temp_s = temp_s->subLink;
            }
        }
        temp_m = temp_m->mainLink;
    }
    if(found == 0)
        printf("\nWord \"%s\" not present in the DATABASE\n", word);
}

/* Save the current database to a backup file */
void save_database(HashTable hashTablle[], char *backup)
{
    if(valid_file_name(backup) == FAILURE)
    {
        fprintf(stderr, "ERROR: Invalid File name\n");
        return;
    }
    FILE *fp = fopen(backup, "w");
    if(fp == NULL)
    {
        fprintf(stderr, "Backup FILE with name %s Could not be created\n", backup);
        return;
    }
    fprintf(fp, "#%s;%s;%s;%s;%s;#\n", "Index", "Word", "FileCount", "FileName", "wordCount");
    for(int i = 0; i < MAX_HASH_SIZE; i++)
    {
        MainNode *temp = hashTablle[i].link;
        while(temp != NULL)
        {
            fprintf(fp,"#%d;%s;%d;", i, temp->word, temp->fileCount);
            SubNode *temp_sub = temp->subLink;
            for(int j = 0; j < temp->fileCount; j++)
            {
                fprintf(fp, "%s;%d;", temp_sub->filename, temp_sub->wordCount);
                temp_sub = temp_sub->subLink;
            }
            fprintf(fp, "#\n");
            temp = temp->mainLink;
        }
    }
    fclose(fp);
    printf("\nINFO: Database saved successfully in file %s\n", backup);
}

/* Update database from a backup file and merge with new file list */
void update_database(FileList **filelist, HashTable hashTablle[], char *backup)
{
    if(valid_file_name(backup) == FAILURE)
    {
        fprintf(stderr, " ERROR: Invalid File name\n");
        return;
    }
    FILE *fp = fopen(backup, "r");
    if(get_file_size(fp) == 0)
    {
        fprintf(stderr, " ERROR: %s file is empty\n", backup);
        return;
    }
    if(valid_database(fp) == FAILURE)
    {
        fprintf(stderr, " ERROR: %s file is not a DATABASE file\n", backup);
        return;
    }
    int index, fileCount; 
    char word[MAX_WORD_LENGTH];

    fscanf(fp, "%*[^\n]");
    fgetc(fp);
    while(fscanf(fp, "#%d;%[^;];%d;", &index, word, &fileCount) == 3)
    {
        MainNode *curr_m = hashTablle[index].link;
        MainNode *prev_m = NULL;

        while(curr_m)
        {
            prev_m = curr_m;
            curr_m = curr_m->mainLink;
        }
        MainNode *newMain = create_mainNode(word, fileCount);
        if(newMain == NULL)
        {
            fprintf(stderr, "\n ERROR: Could not create Database\n");
            return;
        }
        if(prev_m)
            prev_m->mainLink = newMain;
        else
            hashTablle[index].link = newMain;

        SubNode *temp_s = newMain->subLink;

        int wordCount;
        char filename[MAX_FILENAME_LENGTH];

        for(int i = 0; i < fileCount; i++)
        {
            fscanf(fp, "%[^;];%d;", filename, &wordCount);

            if(delete_duplicate(filelist, filename) == SUCCESS)
            {
                printf("\nINFO: Deleting File %s in FileList (already present in the database file %s)\n", filename, backup);
                print_fileList(*filelist);
            }
            SubNode *newSub = create_subNode(filename, wordCount);
            if(newSub == NULL)
            {
                fprintf(stderr, "\n ERROR: Could not create Database\n");
                return;
            }
            if(i == 0)
            {
                newMain->subLink = newSub;
                temp_s = newSub;
            }
            else
            {
                temp_s->subLink = newSub;
                temp_s = newSub;
            }
        }
        fscanf(fp, "#\n");
    }
    fclose(fp);
    if(create_database(*filelist, hashTablle) == FAILURE)
    {
        printf("\nINFO: Database could not be Updated\n");
        return;
    }
    printf("\nINFO: Database Successfully Updated\n");
}

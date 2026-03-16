/***********************************************************************
 *  File name   : validate.c
 *  Description : Validation source file for the Inverted Indexed Search project.
 *                Contains implementations for:
 *                - File size determination
 *                - File argument validation
 *                - Word index calculation
 *                - File name validation
 *                - Database format validation
 *
 *                Functions:
 *                - get_file_size()
 *                - read_and_validate_args()
 *                - get_word_index()
 *                - valid_file_name()
 *                - valid_database()
 ***********************************************************************/

#include "validate.h"

/***********************************************************************
 * Function     : get_file_size
 * Description  : Returns the size of a given file in bytes.
 *                Returns 0 if the file pointer is NULL or if any
 *                seek operation fails.
 * Arguments    : FILE *fp - Pointer to the open file
 * Returns      : size_t   - File size in bytes, or 0 on error
 ***********************************************************************/
size_t get_file_size(FILE *fp)
{
    if (fp == NULL)
        return 0;

    long saved_pos = ftell(fp);
    if (saved_pos == -1)
    {
        fprintf(stderr, "ERROR: ftell failed while saving file position\n");
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "ERROR: fseek to end of file failed\n");
        return 0;
    }

    long size = ftell(fp);
    if (size == -1)
    {
        fprintf(stderr, "ERROR: ftell failed while reading file size\n");
        fseek(fp, saved_pos, SEEK_SET);
        return 0;
    }

    /* Restore the file pointer to where it was before this call */
    if (fseek(fp, saved_pos, SEEK_SET) != 0)
    {
        fprintf(stderr, "ERROR: fseek failed while restoring file position\n");
        return 0;
    }

    return (size_t)size;
}

/***********************************************************************
 * Function     : read_and_validate_args
 * Description  : Validates command-line arguments for file inputs.
 *                Checks for extension, accessibility, duplicates,
 *                emptiness, and adds valid files to the FileList.
 * Arguments    : FileList **filelist - Linked list of files
 *                char **argv         - Command-line arguments
 *                int argc            - Argument count
 * Returns      : int (SUCCESS / FAILURE)
 ***********************************************************************/
int read_and_validate_args(FileList **filelist, char **argv, int argc)
{
    int i = 1, count = 0;

    printf("============================================================\n");
    printf("                 File Validation Summary\n");
    printf("============================================================\n");

    for (; i < argc; i++)
    {
        /* Check that the filename contains an extension at all */
        if (strchr(argv[i], '.') == NULL)
        {
            fprintf(stderr, " INFO: File '%s' has no extension\n", argv[i]);
            continue;
        }

        // Validate that the extension is .txt
        if (valid_file_name(argv[i]) == FAILURE)
        {
            fprintf(stderr, " INFO: File '%s' must have a .txt extension\n", argv[i]);
            continue;
        }

        // Check that the file can actually be opened
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            fprintf(stderr, " INFO: File '%s' could not be opened\n", argv[i]);
            continue;
        }

        // Reject empty files
        if (get_file_size(fp) == 0)
        {
            fprintf(stderr, " INFO: File '%s' is empty\n", argv[i]);
            fclose(fp);
            continue;
        }
        fclose(fp);

        /* Insert into file list. If it is a duplicate, print a warning */
        if (fileList_insert_last(filelist, argv[i]) == DUPLICATE)
        {
            fprintf(stderr, " INFO: File '%s' is in the list already\n", argv[i]);
        }
        else
        {
            printf(" INFO: File '%s' successfully inserted in the FileList\n", argv[i]);
            count++;
        }
    }

    if (count)
        printf("\n              Valid files loaded successfully\n");
    else
        printf("\n             No valid file found in the arguments\n");

    printf("============================================================\n");

    /* Return FAILURE when no valid files were found so main() can exit cleanly */
    if (count == 0)
        return FAILURE;

    return SUCCESS;
}

/***********************************************************************
 * Function     : get_word_index
 * Description  : Returns a hash bucket index based on the first
 *                character of a word.
 *                - 'A'-'Z' or 'a'-'z'  →  0-25
 *                - '0'-'9'             →  26
 *                - Others              →  27
 *                - NULL or empty word  →  -1 (caller must check)
 * Arguments    : char *word - Input word
 * Returns      : int        - Index value, or -1 on invalid input
 ***********************************************************************/
int get_word_index(char *word)
{
    // Guard against NULL pointer or empty string
    if (word == NULL || word[0] == '\0')
        return -1;

    if (word[0] >= 'A' && word[0] <= 'Z')
        return word[0] - 'A';
    if (word[0] >= 'a' && word[0] <= 'z')
        return word[0] - 'a';
    if (word[0] >= '0' && word[0] <= '9')
        return 26;

    return 27;
}

/***********************************************************************
 * Function     : valid_file_name
 * Description  : Validates whether a file name has a ".txt" extension
 *                and a non-empty basename (i.e. rejects ".txt" alone).
 * Arguments    : char *filename - File name string
 * Returns      : int (SUCCESS / FAILURE)
 ***********************************************************************/
int valid_file_name(char *filename)
{
    /* Guard against NULL input — strlen(NULL) is undefined behaviour */
    if (filename == NULL)
        return FAILURE;

    size_t len = strlen(filename);

    /* len < 5 rejects filenames shorter than "a.txt" (5 chars) */
    if (len < 5 || strstr(filename + len - 4, ".txt") == NULL)
        return FAILURE;

    return SUCCESS;
}

/***********************************************************************
 * Function     : valid_database
 * Description  : Validates database file format by checking that the
 *                file starts and ends with a '#' character.
 *                Rewinds the file pointer before returning in all cases.
 * Arguments    : FILE *fp - File pointer to database file
 * Returns      : int (SUCCESS / FAILURE)
 ***********************************************************************/
int valid_database(FILE *fp)
{
    if (fp == NULL)
        return FAILURE;

    /* Check the file is large enough to contain at least two '#' characters
     * fseek(fp, -2, SEEK_END) on a file shorter than 2 bytes is undefined behaviour */
    if (fseek(fp, 0, SEEK_END) != 0)
    {
        rewind(fp);
        return FAILURE;
    }

    long size = ftell(fp);
    rewind(fp);

    if (size < 2)
        return FAILURE;

    // Check first character
    char ch = fgetc(fp);
    if (ch != '#')
    {
        rewind(fp);
        return FAILURE;
    }

    /* Check second-to-last character (last is '\n', the one before it is '#') */
    if (fseek(fp, -2, SEEK_END) != 0)
    {
        rewind(fp);
        return FAILURE;
    }

    ch = fgetc(fp);
    if (ch != '#')
    {
        rewind(fp);
        return FAILURE;
    }

    /* Rewind so the caller can start reading from the beginning */
    rewind(fp);
    return SUCCESS;
}

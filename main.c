/***********************************************************************
 *  File name   : main.c
 *  Description : Entry point for the Inverted Indexed Search Project.
 *                Handles command-line arguments, initializes the hash
 *                table, validates input files, and provides a menu-driven
 *                interface to manage the database.
 *
 *                Menu Options:
 *                1. Create Database
 *                2. Display Database
 *                3. Search Word
 *                4. Save Database
 *                5. Update Database
 *                0. Exit
 *
 *                Functions:
 *                - main()
 *                - initialize_hashTable()
 *                - read_and_validate_args()
 *                - create_database()
 *                - display_database()
 *                - search_word()
 *                - save_database()
 *                - update_database()
 ***********************************************************************/

#include "list.h"
#include "validate.h"
#include "database.h"

int main(int argc, char ** argv)
{
    /* Check if minimum 2 arguments are passed (program name + at least 1 file) */
    if (argc < 2)
    {
        fprintf(stderr, "Insufficient Arguments:\nCorrect Syntax : %s filename.txt filename.txt ...\n", argv[0]);
        return FAILURE;
    }

    FileList *filelist = NULL;                // Linked list to store input file names
    HashTable hashTablle[MAX_HASH_SIZE];      // Hash table for inverted index

    // Initialize the hash table
    initialize_hashTable(hashTablle, MAX_HASH_SIZE);

    /* Validate input files and build the file list */
    if (read_and_validate_args(&filelist, argv, argc) == FAILURE)
        return FAILURE;

    // If no valid files found, exit
    if (filelist == NULL)
    {
        fprintf(stderr, "\nFilelist is Empty. Cannot Create Database\n");  
        return FAILURE;
    }

    // Print the list of valid files
    print_fileList(filelist);

    char choice;                              // User menu choice
    char word[MAX_WORD_LENGTH];               // Word to search
    char filename[MAX_FILENAME_LENGTH];       // Temp filename (unused here, but reserved)
    char backup[MAX_FILENAME_LENGTH];         // Backup file name

    int create_flag = 0, update_flag = 0;     // Flags to restrict duplicate database operations

    FileList *backup_list = NULL;
    // Menu-driven loop
    do
    {
        // Print menu
        printf("\n===== MENU =====\n");
        printf("1. Create Database\n");
        printf("2. Display Database\n");
        printf("3. Search Word\n");
        printf("4. Save Database\n");
        printf("5. Update Database\n");
        printf("0. Exit\n"); 
        printf("Enter choice: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                // Create database only if not already created
                if (create_flag)
                {
                    fprintf(stderr, "\nINFO: Database already created\n");
                    break;
                }
                create_database(filelist, hashTablle);
                create_flag = 1;
                break;

            case '2':
                // Display the database contents
                display_database(hashTablle);
                break; 

            case '3':
                // Search a word in the database
                printf("Enter word to search: ");
                scanf(" %s", word);
                search_word(hashTablle, word);
                break;

            case '4':
                // Save database to backup file
                printf("Enter backup file name to save: ");
                scanf(" %s", backup);
                save_database(hashTablle, backup);
                break;

            case '5':
                /* Update database from a file, only if its not created/updated already */
                if (create_flag)
                {
                    fprintf(stderr, "\nINFO: Database already created. Cannot update Database\n");
                    break;
                }
                printf("Enter the database file to update: ");
                scanf("%s", backup);
                if(delete_duplicate(&backup_list, backup) == SUCCESS)
                {
                    fprintf(stderr, "\nINFO: Database already updated for file %s\n", backup);
                    break;
                }
                fileList_insert_last(&backup_list, backup);
                update_database(&filelist, hashTablle, backup);
                update_flag = 1;
                break;

            case '0':
                // Exit program
                printf("Exiting\n");
                break;

            default:
                // Invalid input handling
                printf("Invalid choice\n");
        }
    } while (choice != '0');

    return 0;
}

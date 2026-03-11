# Inverted Search

A command-line inverted index search engine written in C that reads a set of `.txt` files, builds a word-to-file mapping using a hash table and linked lists, and provides a menu-driven interface to search, display, save, and update the index.

---

## What it does

The program scans one or more `.txt` files passed as command-line arguments and builds an inverted index - a data structure that maps each unique word to the files it appears in and how many times it occurs per file. Once the index is built, the user can interact with it through a menu:

| Option | Description |
|---|---|
| 1 | Create the inverted index from the input files |
| 2 | Display the full index as a formatted table |
| 3 | Search for a word and see which files contain it |
| 4 | Save the current index to a backup file |
| 5 | Load a previously saved index and extend it with new files |
| 0 | Exit |

---

## Project structure

```
.
├── main.c        # Entry point - argument handling, hash table init, menu loop
├── list.h        # Core struct definitions (FileList, MainNode, SubNode, HashTable) and constants
├── list.c        # Linked list and hash table operations - insert, create, delete, print
├── validate.h    # Prototypes for validation and hashing functions
├── validate.c    # File validation, hash function, filename and database format checks
├── database.h    # Prototypes for database operations
├── database.c    # Create, display, search, save, and update the inverted index
├── makefile      # Build system
├── file1.txt        # Sample input files
├── file2.txt
├── file3.txt
└── file4.txt
```

---

## Data structures

The inverted index is built from four structs defined in `list.h`:

**HashTable** — an array of 28 buckets, one per starting character category (a-z maps to 0-25, digits to 26, everything else to 27).

**MainNode** — one node per unique word. Stores the word string, the number of files it appears in, a pointer to the next word at the same hash bucket (collision chaining), and a pointer to its file list.

**SubNode** — one node per file that contains a given word. Stores the filename and the number of times the word appears in that file.

**FileList** — a singly linked list of input filenames, built from the validated command-line arguments.

The in-memory layout looks like this:

```
HashTable[3]          (bucket for words starting with 'd')
    |
    +-- MainNode["data", fileCount=2]
            |-- mainLink --> MainNode["dog", fileCount=1] --> NULL
            |
            +-- SubNode["file1.txt", wordCount=3]
                    |
                    +-- SubNode["file2.txt", wordCount=1] --> NULL
```

---

## Building

Requires GCC and Make.

```bash
make
```

This compiles all `.c` files into object files under `obj/` and links them into `search.out`. Only files that have changed since the last build are recompiled.

To remove all compiled output:

```bash
make clean
```

---

## Usage

```bash
./search.out file1.txt file2.txt file3.txt
```

The program validates each argument before starting. Files that do not exist, are not `.txt`, or are empty are rejected with an informational message. Valid files are loaded into the file list and reported at startup.

**Example session:**

```
./search.out file1.txt file2.txt file3.txt

============================================================
                 File Validation Summary
============================================================
 INFO: File 'file1.txt' successfully inserted in the FileList
 INFO: File 'file2.txt' successfully inserted in the FileList
 INFO: File 'file3.txt' successfully inserted in the FileList

              Valid files loaded successfully
============================================================
FileList: -> file1.txt -> file2.txt -> file3.txt

===== MENU =====
1. Create Database
2. Display Database
3. Search Word
4. Save Database
5. Update Database
0. Exit
Enter choice: 1

INFO: DATABASE successfully created for file file1.txt
INFO: DATABASE successfully created for file file2.txt
INFO: DATABASE successfully created for file file3.txt

Enter choice: 3
Enter word to search: data

Word 'data' is present in (2) file
In File: 'file1.txt' (3) Time
In File: 'file2.txt' (1) Time
```

---

## Implementation notes

**Hash function** — `get_word_index()` in `validate.c` maps a word to a bucket index using only the first character. Uppercase and lowercase letters map to the same index (0-25), so the collision chains at each bucket are walked with `strcmp` to find exact matches.

**Collision handling** — multiple words that hash to the same bucket are stored as a linked list of `MainNode` structs chained via `mainLink`. Search walks this chain using `strcmp` to find the correct word.

**Duplicate prevention** — `create_flag` in `main.c` prevents the database from being created more than once per session. `delete_duplicate()` in `list.c` prevents the same file from being indexed twice during an update operation.

**Save format** — the index is saved to a plain text file using `;` as a field delimiter and `#` as a record boundary. Each record has the form:

```
#<index>;<word>;<fileCount>;<filename>;<wordCount>;...#
```

`valid_database()` in `validate.c` verifies this format before any load or update by checking that the file begins and ends with `#`.

**Update logic** — option 5 loads a saved index from a backup file, reconstructs the hash table from it, removes from the active file list any files already covered by the backup (to avoid double-counting), and then calls `create_database()` on the remaining new files only.

**Memory** — all `FileList`, `MainNode`, and `SubNode` nodes are heap-allocated via `malloc`. The `HashTable` array itself is stack-allocated in `main()` and initialized to `NULL` by `initialize_hashTable()` before use.

---

## Limitations

- Filenames are capped at 19 characters (`MAX_FILENAME_LENGTH = 20` including null terminator). Longer filenames are not supported.
- Words are capped at 19 characters (`MAX_WORD_LENGTH = 20`). Longer tokens read from a file are truncated by `fscanf`.
- The hash function is case-sensitive at the word level. `"Data"` and `"data"` are stored and matched as distinct words.
- Only `.txt` files are accepted as input.
- The index exists in memory only for the duration of the program. Use option 4 to persist it before exiting.

// Somewhat generic functions to allow the main file to focus on problem solving.
#ifndef CRI_IO_HELPERS_H
#define CRI_IO_HELPERS_H

#include <stdbool.h>
#include <stdio.h> // printf family, FILE* family

/**************************** HELPER MACROS ***********************************/

// For my sanity, accepts exactly 1 string literal to be printed.
#define eprintf(msg) fprintf(stderr, __FILE__ ":%i: " msg "\n", __LINE__)

// Grow buffers by this much so we call realloc less often, as that can be slow.
#define BUFFERSIZE 256

// Arbitrary limit to help you avoid allocating like 18 quintillion things.
#define MAXALLOCATIONS 0xFFFF

/******************* DATA TYPES/STRUCTURES DEFINITIONS ************************/

struct StrBuffer {
    char *buffer;
    int index; // Where the last character was written in the buffer.
    int size; // Total allocated memory, usually more than `index`.
};

struct StrVector {
    char **buffer; // 1D array of `char*`'s (are likely heap-allocated).
    int index; // Latest element inserted into `buffer`.
    int size; // Total space allocated for `buffer`, usually more than `index`.
};

/********************** MEMBER "METHOD" PROTOTYPES ****************************/

/**
 * Allocates memory for a StrVector instance to hold the contents of `file`.
 * @param file File to read lines from and store in our vector.
 * @return Handle to your instance, or NULL.
 * @note If any allocation fails, all memory allocated by this is freed.
 */
struct StrVector *cri_strvec_new(FILE *file);

/**
 * Frees all memory associated with your StrVector instance.
 * @note Assumes `vec` itself, `vec->buffer` (and children) are allocated.
 * @warning Has literally 0 error handling. Good luck!
 */
void cri_strvec_delete(struct StrVector *vec);

/**
 * @param sb Preferably the address.
 * @return false on failure to resize the string's buffer.
 * @note Does not free memory failure.
 */
bool cri_strbuf_push(struct StrBuffer *sb, int c);

/** 
 * Inserts `elem` to the latest position in `sv`'s buffer. If there isn't enough 
 * space, it then attempts to resize the buffer.
 * @param vec Preferably the address.
 * @return false on failure to resize the vector's buffer. 
 * @note Does not free memory on failure.
 */
bool cri_strvec_push(struct StrVector *vec, char *elem);

/************************** STATIC INLINE FUNCTIONS ***************************/
// If it's super short that it can be very easily inlined, it goes here.

/** 
 * Short for "is end of line character" or "is line ending character". 
 * Simply checks if `c` is either CR or LF.
 */
static inline bool cri_isendl(int c) 
{
    return (c == '\r') || (c == '\n');
}

/************************** FUNCTION PROTOTYPES *******************************/

/**
 * CR may imply LF, but not always! Account for cases like that via `ungetc`.
 * @param c Last read character.
 * @param stream Where to read the next character from.
 * @note: Windows can use LF in console, but may has CRLF in files.
 */
bool cri_readendl(int c, FILE *stream);

/**
 * @note For file handles, this adjusts the internal file pointer position.
 */
char *cri_readline(FILE *stream);

/**
 * Reads file line by line and stores them in our vector.
 * @param vec Preferably the address.
 * @return false on failure to resize our vector's buffer.
 * @note Does not free memory on failure.
 */
bool cri_readfile(FILE *file, struct StrVector *vec);

/************************ INCLUDE IMPLEMENTATION ******************************/

// Define this macro before #include to get function definitions!
#ifdef CRI_IO_HELPERS_IMPL
#include "helpers.def.h"
#endif // CRIMERAAA_IO_HELPERS_IMPL

/************************** END OF HEADER FILE ********************************/

#endif // CRI_IO_HELPERS_H

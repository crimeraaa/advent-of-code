// Somewhat generic functions to allow the main file to focus on problem solving.
#ifndef CRI_IO_HELPERS_H
#define CRI_IO_HELPERS_H

#include <stdbool.h>
#include <stdio.h> // printf family, FILE* family

/**************************** HELPER MACROS ***********************************/

// For my sanity, accepts exactly 1 string literal to be printed.
#define eprintf(msg) fprintf(stderr, __FILE__ ":%i: " msg "\n", __LINE__)

// Grow buffers by this much so we call realloc less often, as that can be slow.
#define BUFFERSIZE 0xFF

// Arbitrary limit to help you avoid allocating like 18 quintillion things.
#define MAXALLOCATIONS 0xFFFF

/************* STRING BUFFER DATATYPE, "METHOD" PROTOTYPES ********************/

// Work with dynamically-sized strings.
typedef struct StrBuffer {
    char *buffer; // 1D dynamic array of `char`.
    int last; // Index of most recent character pushed back onto `buffer`.
    int size; // Current space allocated for `buffer`, usually `> last`.
} StrBuffer;

/**
 * Initialize an already-declared instance of StrBuffer.
 * @param vec Address of stack-allocated variable or malloc'd pointer.
 * @return false if allocation for `buf->buffer` returned `NULL`, else true.
 */
bool cri_strbuf_init(StrBuffer *vec);

/**
 * @param buf Address of stack-allocated variable or malloc'd pointer.
 * @return false on failure to resize the string's buffer.
 * @note Does not free memory failure. 
 */
bool cri_strbuf_push(StrBuffer *buf, int c);

/************* STRING VECTOR DATATYPE, "METHOD" PROTOTYPES ********************/

// Work with dynamically-sized 1D arrays of strings.
// @note Assumes each string stored in `buffer` is also heap allocated!
typedef struct StrVector {
    char **buffer; // 1D dynamic array of (likely heap-allocated) `char*`'s.
    int last; // Index of most recent pointer pushed back onto `buffer`.
    int size; // Current space allocated for `buffer`, usually `> last`.
} StrVector;

/**
 * Initialize an already-declared instance of StrVector.
 * @param vec Address of stack-allocated variable or malloc'd pointer.
 * @return false if allocation for `vec->buffer` returned `NULL`, else true.
 */
bool cri_strvec_init(StrVector *vec);

/**
 * Frees all memory associated with your StrVector handle.
 * When done, `vec->buffer` is set to NULL.
 * @param vec Address of stack-allocated variable or malloc'd pointer.
 * @note Assumes `vec->buffer` is allocated, calls `cri_strvec_clear`.
 * @warning Assumes `vec` itself isn't `NULL` w/ 0 error handling. Good luck!
 */
void cri_strvec_delete(StrVector *vec);

/**
 * Frees all strings in StrVector's dynamic array, resets `vec->last` to 0
 * and resets `vec->size` to value of `BUFFERSIZE` macro.
 * @param vec Address of stack-allocated variable or malloc'd pointer.
 * @note Assumes `vec->last` is correct. Otherwise, you may leak memory, 
 * or free memory you don't have access to!
 * @todo Make more robust, as is this can prolly end VERY badly.
 */
void cri_strvec_clear(StrVector *vec);

/** 
 * Inserts a string to the latest position in `vec->buffer`. If there isn't 
 * enough space, try to resize `vec->buffer` via a temp pointer and `realloc`.
 * @param vec Address of stack-allocated variable or malloc'd pointer.
 * @return false on failure to resize the vector's buffer. 
 * @note Does not allocate memory for `elem`, does not free memory on failure.
 */
bool cri_strvec_push(StrVector *vec, char *elem);

/************************** STATIC INLINE FUNCTIONS ***************************/
// If it's super short that it can be very easily inlined, it goes here.

/** 
 * Short for "is end of line character" or "is line ending character". 
 * Simply checks if the given stream character `c` is either CR or LF.
 * @note If you got CR, you will want to check for CRLF via `cri_readendl`.
 */
static inline bool cri_isendl(int c) 
{
    return (c == '\r') || (c == '\n');
}

/************************** FUNCTION PROTOTYPES *******************************/

/**
 * CR may imply LF, but not always! Account for cases like that via `ungetc`.
 * @param c Most recently read character from the stream.
 * @param stream Where to read the next character from.
 * @note: Windows can use LF in console, but may has CRLF in files.
 */
bool cri_readendl(int c, FILE *stream);

/**
 * Read 1 line from given `stream` until a line ending character is reached.
 * Accounts for CR, LF and CRLF line endings.
 * @param stream File handle to read from, file pointer will be incremented.
 * @return Dynamically-sized string that stores the line's contents.
 * @note File pointer will probably be pointing to right after the line ending.
 */
char *cri_readline(FILE *stream);

/**
 * Reads file line by line and stores them in a StrVector.
 * @param vec Preferably the address.
 * @return Copied-by-value handle to the StrVector that contains all the lines.
 * @warning For error handling, please check if `vec.buffer` is `NULL`!
 * @note On allocation failure, `vec.buffer` and children, 
 * @note `vec.buffer` itself is set to `NULL` on allocation failure as well.
 * @warning You are responsible for closing the file handle yourself!
 */
StrVector cri_readfile(FILE *file);

/************************ INCLUDE IMPLEMENTATION ******************************/

// Define this macro before #include to get function definitions!
#ifdef CRI_IO_HELPERS_IMPL
#include "helpers.def.h"
#endif // CRI_IO_HELPERS_IMPL

/************************** END OF HEADER FILE ********************************/

#endif // CRI_IO_HELPERS_H

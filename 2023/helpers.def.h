// ! WARNING: This file should only be included once.
// Otherwise, you'll get linker errors due to multiple occurences of symbols!
#ifdef CRI_IO_HELPERS_DEF_H
#error "helpers.def.h was included in more than 1 file."
#else // CRI_IO_HELPERS_DEF_H not defined.
#define CRI_IO_HELPERS_DEF_H

#include <stdbool.h>
#include <stdio.h> // printf family, FILE* family
#include <stdlib.h> // malloc family
#include <string.h> // memcpy family

#include "helpers.h"

// Trying to avoid memory allocations in StrBuffer and StrVector constructors.
#define NO_MALLOC_IN_CONSTRUCTOR 1

/**************** STRING BUFFER "METHOD" IMPLEMENTATIONS **********************/

bool cri_strbuf_init(StrBuffer *buf)
{
    if (buf == NULL) {
        return false; // Error handling for heap-allocated pointers
    }
    buf->last = 0; 
#ifdef NO_MALLOC_IN_CONSTRUCTOR
    buf->size = 0;
    buf->buffer = NULL;
#else
    vec->size = BUFFERSIZE;
    vec->buffer = malloc(sizeof(*vec->buffer) * vec->size);
    return (vec->buffer == NULL);
#endif
    return true;
}

bool cri_strbuf_push(StrBuffer *buf, int c)
{
    // Prepare to resize. (size is 1-based, last is 0-based)
    if (buf->last > buf->size - 1) {
        buf->size += BUFFERSIZE; // grow linearly
        //? Need in case of realloc failure, as original memory is not freed.
        //* Note: put sizeof as first operand to ensure `size_t` math.
        char *temp = realloc(buf->buffer, sizeof(*buf->buffer) * buf->size);
        if (temp == NULL) {
            eprintf("Failed to resize StrBuffer character array.");
            return false;
        } 
        buf->buffer = temp;
    } else if (buf->size > MAXALLOCATIONS) {
        eprintf("Reached StrBuffer's maximum string length!");
        fprintf(stderr, "%i chars / %i allowed.\n", buf->size, MAXALLOCATIONS);
        return false;
    }
    buf->buffer[buf->last++] = (char)c;
    return true;
}

/**************** STRING VECTOR "METHOD" IMPLEMENTATIONS **********************/

bool cri_strvec_init(StrVector *vec)
{
    if (vec == NULL) {
        return false; // Error handling for heap-allocated pointers
    }
    vec->last = 0; 
#ifdef NO_MALLOC_IN_CONSTRUCTOR
    vec->size = 0;
    vec->buffer = NULL;
#else
    vec->size = BUFFERSIZE;
    vec->buffer = malloc(sizeof(*vec->buffer) * vec->size);
    return (vec->buffer == NULL);
#endif
    return true;
}

void cri_strvec_delete(StrVector *vec)
{   
    cri_strvec_clear(vec);
    free(vec->buffer);
    vec->buffer = NULL; // erase so user can't poke at it later
}

void cri_strvec_clear(StrVector *vec)
{
    for (int i = 0; i < vec->last; i++) {
        free(vec->buffer[i]);
    }
    vec->last = 0;
#ifdef NO_MALLOC_IN_CONSTRUCTOR
    vec->size = 0;
#else
    vec->size = BUFFERSIZE;
#endif
}

bool cri_strvec_push(StrVector *vec, char *elem)
{
    // Prepare to resize (size is 1-based, last is 0-based)
    // Could use `>= size` instead of `> size - 1`, to avoid unsigned overflow 
    // when size == 0
    if (vec->last > vec->size - 1) {
        vec->size += BUFFERSIZE; // grow linearly
        // ? Need in case of realloc failure, as original memory is not freed.
        // * Note: put sizeof as first operand to ensure `size_t` math.
        char **temp = realloc(vec->buffer, sizeof(*vec->buffer) * vec->size);
        if (temp == NULL) {
            eprintf("Could not resize StrVector's pointer array.");
            return false;
        }
        vec->buffer = temp;
    } else if (vec->size > MAXALLOCATIONS) {
        eprintf("Reached StrVector's maximum allowable allocations!");
        fprintf(stderr, "%i pointers / %i allowed.\n", vec->size, MAXALLOCATIONS);
        return false;
    }
    vec->buffer[vec->last++] = elem;
    return true;
}

/*************************  FUNCTION IMPLEMENTATION ***************************/

bool cri_readendl(int c, FILE *stream)
{
    if (c == '\r' && (c = fgetc(stream)) != EOF) {
        // Char after the CR wasn't LF and for ungetc: EOF == can't pushback 
        if (c != '\n' && (ungetc(c, stream)) == EOF) { 
            eprintf("Failed to read line-ending with CR ('\\r').");
            return false;
        }
    }
    return true; // probably got lone LF or successfully read CRLF
}

char *cri_readline(FILE *stream)
{
    // Locally scope struct definition: https://stackoverflow.com/a/5590795
    StrBuffer line; 
    cri_strbuf_init(&line);
    int c; // `int` so we can check for `EOF` (macro, usually expands to `-1`).
    while ((c = fgetc(stream)) != EOF && !cri_isendl(c)) {
        if (!cri_strbuf_push(&line, c)) {
            free(line.buffer);
            return NULL;
        }
    }
    line.buffer[line.last] = '\0'; // no need increment is correct index alr.
    if (!cri_readendl(c, stream)) {
        free(line.buffer);
        return NULL;
    } 
    // (Try to) tightly pack our string buffer with space for NUL termination.
    char *copy = malloc(line.last + 1);
    if (copy == NULL) {
        free(line.buffer);
        eprintf("Failed to minimize read line's character buffer.");
        return NULL;
    }
    copy = memcpy(copy, line.buffer, line.last + 1); // Faster than strcpy?
    free(line.buffer);
    return copy;
}

StrVector cri_readfile(FILE *file)
{
    // Copy by value so to avoid less heap allocations
    StrVector vec; 
    cri_strvec_init(&vec);

    long START_POSITION = ftell(file);
    // Move file pointer to EOF, a.k.a. last possible file pointer position.
    fseek(file, 0, SEEK_END);  

    long EOF_POSITION = ftell(file);
    // Move file pointer back to starting position so we can read properly.
    fseek(file, 0, START_POSITION); 

    // Use `<` instead of `!=`, juuust in case shit hits the fan
    while (ftell(file) < EOF_POSITION) {
        char *line = cri_readline(file);
        // Be terrible to insert NULL as a line, so just don't!
        if (line == NULL) {
            cri_strvec_delete(&vec);
            eprintf("Could not read 1 or more input file line/s.");
            return vec;
        }
        // Continuing if this failed would be horrible, so just don't!
        if (!cri_strvec_push(&vec, line)) {
            cri_strvec_delete(&vec);
            eprintf("Could not insert line to string vector buffer.");
            return vec;
        }
    }
    return vec; // copy by value, it's just a pointer and 2 ints anyway
}

#endif // CRI_IO_HELPERS_DEF_H

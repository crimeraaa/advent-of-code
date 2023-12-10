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

/******************** MEMBER "METHOD" IMPLEMENTATION **************************/

struct StrVector *cri_strvec_new(void)
{
    struct StrVector *vec = malloc(sizeof(*vec));
    if (vec == NULL) {
        return NULL;
    }
    vec->index = 0; 
    vec->size = BUFFERSIZE;
    vec->buffer = malloc(sizeof(*(vec->buffer)) * vec->size);
    if (vec->buffer == NULL) {
        free(vec);
        eprintf("Failed to allocate memory for vector buffer.");
        return NULL;
    }
    return vec;
}

void cri_strvec_delete(struct StrVector *vec)
{
    for (int i = 0; i < vec->index; i++) {
        char *str = vec->buffer[i];
        free(str);
    }
    free(vec->buffer);
    free(vec);
}

bool cri_strvec_push(struct StrVector *vec, char *elem)
{
    // Prepare to resize (size is 1-based, index is 0-based)
    if (vec->index > vec->size - 1) {
        vec->size += BUFFERSIZE; // grow linearly
        // Need this cause on realloc failure, original memory not freed
        char **temp = realloc(vec->buffer, vec->size);
        if (temp == NULL) {
            return false;
        }
        vec->buffer = temp;
    } else if (vec->size > MAXALLOCATIONS) {
        return false;
    }
    vec->buffer[vec->index++] = elem;
    return true;
}

bool cri_strbuf_push(struct StrBuffer *str, int c)
{
    // Prepare to resize. (size is 1-based, index is 0-based)
    if (str->index > str->size - 1) {
        str->size += BUFFERSIZE; // grow linearly
        // Need this cause on realloc failure, original memory not freed
        char *temp = realloc(str->buffer, str->size);
        if (temp == NULL) {
            return false;
        } 
        str->buffer = temp;
    } else if (str->size > MAXALLOCATIONS) {
        return false;
    }
    str->buffer[str->index++] = (char)c;
    return true;
}

/*************************  FUNCTION IMPLEMENTATION ***************************/

bool cri_readendl(int c, FILE *stream)
{
    if (c == '\r' && (c = fgetc(stream)) != EOF) {
        // Char after the CR wasn't LF and for ungetc: EOF == can't pushback 
        if (c != '\n' && (ungetc(c, stream)) == EOF) { 
            eprintf("Failed to read line-ending with CR ('\\r') .");
            return false;
        }
    }
    return true; // probably got lone LF or successfully read CRLF
}

char *cri_readline(FILE *stream)
{
    // Locally scope struct definition: https://stackoverflow.com/a/5590795
    struct StrBuffer line;
    line.index = 0;
    line.size = BUFFERSIZE;
    // Put sizeof as first operand to ensure size_t math.
    line.buffer = malloc(sizeof(*line.buffer) * line.size);
    if (line.buffer == NULL) {
        eprintf("Failed to allocate memory for buffer.");
        return NULL;
    }

    int c; // Use `int` so can check for EOF (macro for `-1`).

    // EOF indicates error, while CR or LF indicates EOL
    while ((c = fgetc(stream)) != EOF && !cri_isendl(c)) {
        if (!cri_strbuf_push(&line, c)) {
            free(line.buffer);
            eprintf("Failed to resize string buffer.");
            return NULL;
        }
    }
    line.buffer[line.index++] = '\0';

    if (!cri_readendl(c, stream)) {
        free(line.buffer);
        return NULL;
    } 

    // (Try to) tightly pack our string buffer with space for NUL termination.
    char *copy = malloc(line.index + 1);
    if (copy != NULL) {
        // Supposedly faster than strcpy, forgot where I heard that from though.
        copy = memcpy(copy, line.buffer, line.index + 1);
    } else {
        eprintf("Failed to tightly pack string buffer.");
    }

    free(line.buffer); // Freed regardless of status of `copy`.
    return copy;
}

bool cri_readfile(FILE *file, struct StrVector *vec)
{
    long START_POSITION, EOF_POSITION;
    START_POSITION = ftell(file);
    fseek(file, 0, SEEK_END);
    EOF_POSITION = ftell(file);

    // Reset file pointer so we can read data properly.
    fseek(file, 0, START_POSITION); 

    // Assume we're reading a plaintext file using only ASCII chars
    while (ftell(file) != EOF_POSITION) {
        char *line = cri_readline(file);
        if (!cri_strvec_push(vec, line)) {
            eprintf("Could not read input file completely.");
            return false;
        }
    }
    return true;
}

#endif // CRI_IO_HELPERS_DEF_H

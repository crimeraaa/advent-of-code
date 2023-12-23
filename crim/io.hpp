#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * `__FILE__` and `__FILE__` are expanded at compile-time.
 * This gets the correct line numbers at the call-site.
 * Otherwise, these macros would be expanded only in `log_error`'s body.
 */
#define crim_log_error(...) crim::log_error(__FILE__, __LINE__, __VA_ARGS__)

namespace crim {
    static inline void log_error(const char *file, int line, const char *fmt, ...)
    {
        va_list argp;
        va_start(argp, fmt);
        fprintf(stderr, "%s:%i: ", file, line);
        vfprintf(stderr, fmt, argp);
        va_end(argp);
    }


    // Determine if `c` is a line-ending character (one of CR or LF).
    static inline bool isendl(int c) {
        return c == '\n' || c == '\r';
    }

    /**
     * If we got a carriage return (CR), try to read line-feed (LF). 
     * This is to check for Windows-style CRLF line-endings.
     * @note It's possible (though unlikely) to get lone CR line-ending.
     * In that case we try to return the nexr read character to the stream.
     */
    static inline bool readendl(int c, FILE *stream) {
        // Got CR and successfully read the next character
        if (c == '\r' && (c = fgetc(stream)) != EOF) {
            // Next char wasn't LF and we couldn't return it to the stream
            if (c != '\n' && (ungetc(c, stream) == EOF)) {
                return false;
            }
        }
        // Had LF to begin with / read CRLF / got CR only and returned a char
        return true;
    }

    /**
     * Reads a line of text, up until the first line-ending character, and stores
     * the contents in a nul-terminated buffer.
     * @param stream Where the read from, e.g. `stdin` or a file handle. 
     */
    char *readline(FILE* stream) {
        char *buffer = NULL;
        size_t index = 0;
        size_t capacity = 0;
        int c; // use `int` so we can check for EOF
        while ((c = fgetc(stream)) != EOF && !isendl(c)) {
            // buffer.push_back(c);
            if (index + 1 > capacity) {
                capacity += 128;
                char *tmp = (char*)realloc(buffer, sizeof(*buffer) * capacity);
                if (tmp == NULL) {
                    crim_log_error("Failed to resize buffer!\n");
                    free(buffer);
                    return NULL;
                }
                buffer = tmp;
            }
            buffer[index++] = (char)c;
        }
        buffer[index] = '\0';
        if (!readendl(c, stream)) {
            crim_log_error("Failed to read line ending!");
            free(buffer);
            return NULL;
        }
        return buffer;
    }

    /**
     * Prompts user to enter a line of text into the standard input stream.
     * Their inputs are stored in a heap-allocated `char` pointer.
     * @param fmt C string literal (`char*`) which can be a format string. 
     * @param ... Format specifier arguments to the format string, if any.
     */
    char *get_string(const char *fmt, ...) {
        va_list argp;
        va_start(argp, fmt);
        vfprintf(stdout, fmt, argp);
        va_end(argp);
        return readline(stdin);
    }
};

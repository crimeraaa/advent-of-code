#pragma once

#include "string.hpp"
#include <cstdarg>
#include <cstdio>

/**
 * `__FILE__` and `__FILE__` are expanded at compile-time.
 * This gets the correct line numbers at the call-site.
 * Otherwise, these macros would be expanded only in `log_error`'s body.
 */
#define crim_log_error(...) crim::log_error(__FILE__, __LINE__, __VA_ARGS__)

namespace crim {
    // [See here](https://stackoverflow.com/a/16513259) for more information.
    // Instead of throwing errors, though, I prefer to return empty buffers.
    // class io_read_error : public std::runtime_error {
    // public:
    //     io_read_error(const char *msg) : std::runtime_error(msg) {}
    // };

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
     * @param buffer Where to store the data.
     * @note On failure to read line-ending, returns an empty string (size 0).
     * This may be different if user entered an empty string (size 1).
     */
    string readline(FILE* stream) {
        string buffer;
        int c; // use `int` so we can check for EOF
        while ((c = fgetc(stream)) != EOF && !isendl(c)) {
            buffer.push_back(c);
        }

        // Return-value-optimization may not happen when there are 2 or more,
        // possible values (especially named) that can be returned.
        // See here: https://en.wikipedia.org/wiki/Copy_elision#RVO
        // 
        // So as a result, I've resorted to this.
        if (!readendl(c, stream)) {
            crim_log_error("Failed to read line ending!");
            buffer.clear();
        } else {
            buffer.push_back('\0');
        }
        return buffer;
    }

    /**
     * Prompts the user for a line of `char`s from the standard input stream.
     * @param fmt String literal or format string for the prompt.
     * @param ... Arguments to format string, if any.
     * @return `crim::string` container of the line that was read.
     * @note Benefits from return-value optimization in its current state.
     */
    string get_string(const char *fmt, ...) {
        va_list argp;
        
        // verbose and stupid C-style varargs, but this just prints the prompt
        va_start(argp, fmt);
        vfprintf(stdout, fmt, argp);
        va_end(argp);

        // yay return-value-optimization
        return readline(stdin);
    }

    dyarray<string> readfile(const char *filename) {
        dyarray<string> buffer;
        FILE* handle = fopen(filename, "r");
        if (handle) {
            long FILESTART = ftell(handle);
            fseek(handle, 0, SEEK_END);
            long FILEEND = ftell(handle);
            fseek(handle, 0, SEEK_SET); // bring back to start so can read properly

            while (ftell(handle) < FILEEND) {
                // Destructor gets called -_-
                buffer.push_back(readline(handle));
            }
            fclose(handle);
        }
        return buffer;
    }
};

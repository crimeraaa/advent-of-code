#pragma once

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <new>

#include "logerror.hpp"
#include "memory.tcc"
#include "base_string.tcc"

// Don't forget to `#undef` this at the end of this file, others need it!
#define crim_logerror(func, info) crim_logerror_nofunc("crim", func, info)

namespace crim {
    bool isendl(int ch) noexcept {
        return ch == '\r' || ch == '\n';
    }
    
    bool readcrlf(std::FILE *p_stream, int ch) {
        if (ch == '\r' && (ch = std::fgetc(p_stream)) != EOF) {
            if (ch != '\n' && std::ungetc(ch, p_stream) == EOF) {
                crim_logerror("readcrlf", "std::ungetc() failed!");
                return false;
            }
        }
        return true;
    }
    
    // TODO: Clear stdin stream on errors
    cstring readline(std::FILE *p_stream) {
        cstring input;
        int ch;

        while ((ch = std::fgetc(p_stream)) != EOF && !isendl(ch)) {
            if (!input.push_back(static_cast<char>(ch))) {
                crim_logerror("readline", "input.push_back() failed!");
                break;
            }    
        } 
        if (!readcrlf(p_stream, ch)) {
            crim_logerror("readline", "crim::readcrlf() failed!");
        }
        return input;
    }
    
    cstring get_string(const char *p_fmts, ...) {
        std::va_list p_args;
        va_start(p_args, p_fmts);
        std::vfprintf(stdout, p_fmts, p_args);
        va_end(p_args);
        return readline(stdin);
    }
};

#undef crim_logerror

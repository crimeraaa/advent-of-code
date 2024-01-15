#pragma once

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <new>

#include "logerror.hpp"
#include "allocator.tcc"
#include "base_string.tcc"

#define crim_input_logerror(info) crim_logerror("", info)

namespace crim {
    bool isendl(int ch) {
        return ch == '\r' || ch == '\n';
    }
    
    bool readcrlf(std::FILE *p_stream, int ch) {
        if (ch == '\r' && (ch = std::fgetc(p_stream)) != EOF) {
            if (ch != '\n' && std::ungetc(ch, p_stream) == EOF) {
                crim_input_logerror("std::ungetc() failed!");
                return false;
            }
        }
        return true;
    }
    
    cstring readline(std::FILE *p_stream) {
        cstring input;
        int ch;

        while ((ch = std::fgetc(p_stream)) != EOF && !isendl(ch)) {
            if (!input.push_back(static_cast<char>(ch))) {
                crim_input_logerror("input.push_back() failed!");
                break;
            }    
        } 
        if (!readcrlf(p_stream, ch)) {
            crim_input_logerror("crim::readcrlf() failed!");
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


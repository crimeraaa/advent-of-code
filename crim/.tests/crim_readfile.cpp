/* -*- C -*- */
#include <stdarg.h>
#include <stdio.h>

/* -*- C++ -*- */
#include <type_traits>

/* -*- MY DATA STRUCTURES -*- */
#include <crim/dystring.tcc>
#include <crim/dyarray.tcc>

/* -*-  READFILE TEST -*- */
#include "readfile.hpp"
/* -*-                -*- */

crim::string readline(FILE *stream) {
    crim::string in_buffer;
    int c;
    // Keep reading input stream until some line-ending character is reached
    while ((c = fgetc(stream)) != EOF && c != '\r' && c != '\n') {
        in_buffer.push_back(static_cast<char>(c));
    }
    // Check for CRLF or lone CR
    if (c == '\r' && (c = fgetc(stream)) != EOF) {
        if (c != '\n' && ungetc(c, stream) == EOF) {
            perror("Failed to read CRLF line ending!");
            in_buffer.clear();
            return in_buffer;
        }
    }
    // Moves might optimize out nul char appends, so append this explicitly!
    // Otherwise Valgrind will catch us trying to read uninitialized memory.
    in_buffer.append('\0'); 
    return in_buffer; // return-value optimization requires raw returns.
}

crim::string get_string(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stdout, fmt, argp);
    va_end(argp);
    return readline(stdin);
}

crim::dyarray<crim::string> readfile(const char *fname) {
    crim::dyarray<crim::string> contents;
    FILE *infile = fopen(fname, "r");
    if (infile == NULL) {
        perror("fopen");
        return contents; // return empty vector
    }
    while (!feof(infile)) {
        // Pushing back via move-assignment since result is an rvalue.
        // result of readline here is an rvalue so we call the && version
        contents.push_back(readline(infile));

        // Pushing back via copy-assignment since `s` is an lvalue.
        // crim::string s = readline(file); contents.push_back(s);
    }
    fclose(infile);
    return contents; // return-value optimization requires raw returns.
}

void input_test() {
    auto name = get_string("What's your name? ");
    auto food = get_string("Hi %s! What's your favorite food? ", name.data());
    printf("Nice to meet you %s! We both love %s!\n", name.c_str(), food.c_str());
}

void array_test() {
    crim::dyarray<crim::string> v;
    v.push_back("1").push_back("2").push_back("3").push_back("4");
    v.push_back("5").push_back("6").push_back("7").push_back("8");
    v.push_back("9").push_back("10").push_back("11").push_back("12");
    v.push_back("13").push_back("14").push_back("15").push_back("16");
    v.push_back("17");
    int padding = get_digits(v.length());
    for (size_t i = 0; i < v.length(); i++) {
        printf("%*zu: %s (%p)\n", padding, i + 1, v[i].c_str(), (void*)v[i].data());
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    // input_test();
    array_test();
    const char *fname = (argc == 2) ? argv[1] : FALLBACK "segfault.txt";
    auto contents = readfile(fname);
    int padding = get_digits(contents.length());
    for (size_t i = 0; i < contents.length(); i++) {
        printf("%*zu: %s\n", padding, i + 1, contents[i].c_str());
    }
    return 0;
}

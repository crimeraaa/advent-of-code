#include <crim/dystring.tcc>
#include <crim/dyarray.tcc>
#include <stdarg.h>
#include <stdio.h>
#include <type_traits>

#include "readfile.hpp"

crim::string readline(FILE *stream) {
    crim::string in_buffer;
    int c;
    // Keep reading input stream until some line-ending character is reached
    while ((c = fgetc(stream)) != EOF && c != '\r' && c != '\n') {
        in_buffer.push_back((char)c);
    }
    // Check for CRLF or lone CR
    if (c == '\r' && (c = fgetc(stream)) != EOF) {
        if (c != '\n' && ungetc(c, stream) == EOF) {
            perror("Failed to read CRLF line ending!");
            in_buffer.clear();
            return in_buffer;
        }
    }
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
        perror("Could not open requested input file");
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

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    crim::string s;
    crim::dyarray<crim::string> v;
    // const char *fname = (argc == 2) ? argv[1] : FALLBACK "segfault.txt";
    // auto contents = readfile(fname);
    // int padding = get_digits(contents.length());
    // for (size_t i = 0; i < contents.length(); i++) {
    //     printf("%*zu: %s\n", padding, i + 1, contents[i].c_str());
    // }
    return 0;
}

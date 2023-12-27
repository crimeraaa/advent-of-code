#include <crim/dystring.tcc>
#include <crim/dyarray.tcc>
#include <stdarg.h>
#include <stdio.h>
#include <type_traits>

#define TEST_READFILE_FUNCTION

#ifdef _WIN32
#define FALLBACK "C:/users/crimeraaa/repos/advent-of-code/crim/.tests/"
#else
#define FALLBACK "/home/crimeraaa/repos/advent-of-code/crim/.tests/"
#endif

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
    crim::dyarray<crim::string> in_buffer;
    FILE *file = fopen(fname, "r");
    if (file == NULL) {
        perror("Could not open file");
        return in_buffer;
    }
    while (!feof(file)) {
        // Pushing back via move-assignment since result is an rvalue.
        // result of readline here is an rvalue so we call the && version
        in_buffer.push_back(readline(file));

        // Pushing back via copy-assignment since `s` is an lvalue.
        // crim::string s = readline(file); in_buffer.push_back(s);
    }
    fclose(file);
    return in_buffer; // return-value optimization requires raw returns.
}

template<typename IntT> int get_digits(IntT value) {
    int digits = 0;
    if constexpr(std::is_signed<IntT>::value) {
        value *= (value >= 0) ? 1 : -1;
    }
    while (value > 0) {
        digits++;
        value /= 10;
    }
    return digits;
}

int main(int argc, char *argv[]) {
    const char *fname = (argc == 2) ? argv[1] : FALLBACK "segfault.txt";

#ifdef TEST_READFILE_FUNCTION
    // Segfaults when reading this file and the Makefile. I wonder why?
    auto contents = readfile(fname);
    int padding = get_digits(contents.length());
    for (size_t i = 0; i < contents.length(); i++) {
        printf("%*zu: %s\n", padding, i + 1, contents[i].c_str());
    }
#else
    // So reading the file line-by-line here is ok, hmm...
    FILE *file = fopen(fname, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }
    // It seems something is wrong with my push_back function.
    crim::dyarray<crim::string> contents;
    fseek(file, 0, SEEK_END);
    const long POS_EOF = ftell(file);
    fseek(file, 0, SEEK_SET);
    while (ftell(file) < POS_EOF) {
        contents.push_back(readline(file));
    }
    fclose(file);

    for (size_t i = 0; i < contents.length(); i++) {
        printf("%2zu: %s\n", i + 1, contents[i].c_str());
    }
#endif
    return 0;
}

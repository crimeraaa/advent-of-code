#include <stdio.h>

#include <vector>
#include <string>
#include <fstream>
#include <type_traits>

#include "readfile.hpp"

std::string readline(FILE *stream) {
    std::string in;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\r' && c != '\n') {
        in.push_back(c);
    }
    if (c == '\r' && (c = fgetc(stream)) != EOF) {
        if (c != '\r' && ungetc(c, stream) == EOF) {
            in.clear();
        }
    }
    in.push_back('\0');
    return in;
}

std::vector<std::string> readfile(const char *fname) {
    std::vector<std::string> contents;
#ifdef STD_READFILE_USE_FILEPTR
    FILE *file = fopen(fname, "r");
    if (file == NULL) {
        perror("Failed to open requested input file");
        return contents; // return empty vector
    }
    while (!feof(file)) {
        contents.push_back(readline(file));
    }
    fclose(file);
#else
    std::ifstream infile(fname);
    // See: https://stackoverflow.com/a/12133398
    for (std::string line; std::getline(infile, line);) {
        contents.push_back(std::move(line)); // line will be overwritten anyway
    }
    infile.close();
#endif
    return contents;
}

int main(int argc, char *argv[]) {
    // (void)argc; (void)argv;
    const char *fname = (argc == 2) ? argv[1] : FALLBACK "segfault.txt";
    auto contents = readfile(fname);
    int padding = get_digits(contents.size());
    for (size_t i = 0; i < contents.size(); i++) {
        printf("%*zu: %s\n", padding, i + 1, contents[i].c_str());
    }
    return 0;
}

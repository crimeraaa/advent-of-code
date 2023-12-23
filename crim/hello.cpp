#include "io.hpp"
#include "string.hpp"

crim::string make_copy(const crim::string &source) {
    printf("[make_copy] source is: \"%s\"\n", source.data());
    return source; // implicitly calls copy-constructor
}

int main(int argc, char *argv[]) {
    crim::string name = "Hi mom!";
    printf("name: \"%s\"\n", name.data());
    crim::string copy = make_copy(name).append(" Hello there!");
    printf("copy: \"%s\"\n", copy.data());
    return 0;
}

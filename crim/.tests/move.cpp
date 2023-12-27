#include <crim/dystring.tcc>
#include <stdio.h>

crim::string literal(const char *msg) {
    return crim::string(msg);
}

void print_string(const crim::string &src, int i) {
    printf(
        "{s%i}:      \"%s\"\n"
        "{length}:   %zu\n"
        "{capacity}: %zu\n"
        "\n"
        , i
        , src.c_str()
        , src.length()
        , src.capacity()
    );
}

int main() {
    crim::string s1 = literal("Hi mom!"); // basic constructor
    crim::string s2 = "Hi dad!"; // basic constructor
    const crim::string s3 = s2; // copy-constructor
    print_string(s1, 1);
    print_string(s2, 2);
    print_string(s3, 3);
    s1 = literal("Hello there!"); // move-assignment!
    s2 = s1; // copy-assignment
    s2.pop_back();
    // s3 = s2; // compile error but only cuz no such assignment operator exists
    print_string(s1, 1);
    print_string(s2, 2);
    print_string(s3, 3);
    return 0;
}

#include "io.hpp"
#include "base_dyarray.hpp"
#include "dyarray.hpp"
#include "dystring.hpp"
#include <initializer_list>

static inline const char *tostring(bool b) {
    return b ? "true" : "false";
}

void chaintest();
void lengthtest1();
void lengthtest2();
void assignmenttest();
void operatortest();
void resizetest();
void looptest();

int main(int argc, char *argv[]) {
    looptest();
    return 0;
}

void chaintest() {
    crim::string name = "Hi mom!";
    printf("%s\n", name.data());
    printf("%s\n", name.append(" I'm making a library! Woohoo!!!!!").data());
    printf("{name} is empty? %s\n", tostring(name.empty()));
    crim::string blank;
    printf("{blank} is empty? %s\n", tostring(blank.empty()));
    crim::string shorten = "Shorten me!";
    printf("{shorten}: \"%s\"\n", shorten.data());
    printf("{resized}: \"%s\"\n", shorten.resize(shorten.length() - 4).append("!").data());
    for (const auto &c : shorten) {
        printf("%c ", c);
    }
    printf("\n");
}

void lengthtest1() {
    crim::string test;
    printf("(default constructor) length: %zu\n", test.length());
    printf("(default constructor) empty?: %i\n", test.empty());
    test = "";
    printf("(empty string assign) length: %zu\n", test.length());
    printf("(empty string assign) empty?: %i\n", test.empty());
    crim::string test2 = "";
    printf("(empty constructor) length: %zu\n", test.length());
    printf("(empty constructor) empty?: %i\n", test.empty());
}

void lengthtest2() {
    crim::string hello = "Hi mom!";
    crim::string dummy;
    printf("(dummy) length: %zu\n", dummy.length());
    printf("(dummy) empty?: %i\n", dummy.empty());
    printf("(hello) length: %zu\n", hello.length());
    printf("(hello) empty?: %i\n", hello.empty());
    printf("%s\n", hello.data());
    for (const auto &c : hello) {
        printf("%c ", c);
    }
    printf("\n");
}

void assignmenttest() {
    crim::string s1 = "Hi mom!";
    crim::string s2 = s1;
    crim::string s3;
    s3 = s1;
    printf("(s1): %s, (s2): %s, (s3): %s\n", s1.data(), s2.data(), s3.c_str());
    printf("(s1): %p, (s2): %p, (s3): %p\n", (void*)s1.data(), (void*)s2.data(), (void*)s3.data());
}

void operatortest() {
    crim::string test;
    crim::string copy;
    printf("Test: %s\n", test.c_str()); // is empty string so should be ok
    test += "Hi mom!"; copy = test;
    printf("Test: %s, Copy: %s\n", test.c_str(), copy.c_str());
    test = "Hello there!"; copy += test;
    printf("Test: %s, Copy: %s\n", test.c_str(), copy.c_str());
    copy = "This is a really long string that will cause reallocation.";
    copy += " Here is more data that will hopefully let me check for bugs!";
    printf("Copy: %s\n", copy.c_str());
}

void resizetest() {
    crim::string test = "Hi mom!";
    printf("last char: %i\n", test.at(test.length()));
    printf("last char: %i\n", test.resize(test.length()).at(test.length()));
}

void looptest() {
    crim::string test = "Hi mom!";
    // Does not include nul char as `.end()` points to it most likely.
    for (const auto &c : test) {
        printf("%c (%i)\n", c, c);
    }
    printf("\n");

    crim::dyarray<int> list = {1, 2, 3, 4, 5};
    list.push_back(7);
    // Should be false regardless if we push back or not, and it is!
    printf("(list) empty?: %i\n", list.empty());
    for (const auto &i : list) {
        printf("%i, ", i);
    }
    printf("\n");

    crim::dyarray<int> none = {};
    // none.push_back(14);
    // When above line is commented out, this is true as expected! :)
    printf("(none) empty?: %i\n", none.empty());
    for (const auto &i : none) {
        printf("%i, ", i);
    }
    printf("\n");
}

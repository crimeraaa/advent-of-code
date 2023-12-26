#include "io.hpp"
#include "base_dyarray.hpp"
#include "dyarray.hpp"
#include "dystring.hpp"
#include <initializer_list>
#include <iostream>

static inline const char *tostring(bool b) {
    return b ? "true" : "false";
}

void dystring_chain_test();
void dystring_length_test1();
void dystring_length_test2();
void dystring_assign_test();
void dystring_operator_test();
void dystring_resize_test();
void dystring_foreach_test();
void dystring_copyctor_test();
void dystring_movector_test();
void dystring_bothctor_test() {
    crim::string s;
    s = crim::string("Hi mom!"); // move-assignment

    crim::string t;
    t = s; // copy-assignment
    printf("{s} = \"%s\"\n", s.data());
    printf("{t} = \"%s\"\n", t.data());
}

void dyarray_bothctor_test() {
    using crim::dyarray;
    dyarray<int> vx = {1, 2, 3, 4};
}

int main() {
    // DYSTRING TESTS
    dystring_chain_test();
    dystring_length_test1();
    dystring_length_test2();
    dystring_assign_test();
    dystring_operator_test();
    dystring_resize_test();
    dystring_foreach_test();
    dystring_copyctor_test();
    dystring_movector_test();
    dystring_bothctor_test();

    // DYARRAY TESTS
    dyarray_bothctor_test();
    return 0;
}

void dystring_chain_test() {
    crim::string name = "Hi mom!";
    printf("%s\n", name.data());
    printf("%s\n", name.append(" I'm making a library! Woohoo!!!!!").data());
    printf("{name} is empty? %s\n", tostring(name.empty()));
    crim::string blank;
    printf("{blank} is empty? %s\n", tostring(blank.empty()));
}

void dystring_length_test1() {
    crim::string test;
    printf("(default constructor) length: %zu\n", test.length());
    printf("(default constructor) empty?: %s\n", tostring(test.empty()));
    test = "";
    printf("(empty string assign) length: %zu\n", test.length());
    printf("(empty string assign) empty?: %s\n", tostring(test.empty()));
    crim::string test2 = "";
    printf("(empty constructor) length: %zu\n", test.length());
    printf("(empty constructor) empty?: %s\n", tostring(test.empty()));
}

void dystring_length_test2() {
    crim::string hello = "Hi mom!";
    crim::string dummy;
    printf("(dummy) length: %zu\n", dummy.length());
    printf("(dummy) empty?: %s\n", tostring(dummy.empty()));
    printf("(hello) length: %zu\n", hello.length());
    printf("(hello) empty?: %s\n", tostring(hello.empty()));
    printf("%s\n", hello.data());
    for (const auto &c : hello) {
        printf("%c ", c);
    }
    printf("\n");
}

void dystring_assign_test() {
    crim::string s1 = "Hi mom!";
    crim::string s2 = s1;
    crim::string s3;
    s3 = s1;
    printf("(s1): %s, (s2): %s, (s3): %s\n", s1.data(), s2.data(), s3.c_str());
    printf("(s1): %p, (s2): %p, (s3): %p\n", (void*)s1.data(), (void*)s2.data(), (void*)s3.data());
}

void dystring_operator_test() {
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

void dystring_resize_test() {
    crim::string test = "Hi mom!";
    printf("{resize_test}  test:     \"%s\"\n", test.data());
    printf("{resize_test}  length:    %zu\n", test.length());
    printf("{resize_test}  capacity:  %zu\n", test.capacity());
    printf("{resize_test}  last char: %i\n", test.at(test.length()));
    // printf("last char: %i\n", test.resize(test.length()).at(test.length()));
    // test = test.resize(test.length() - sizeof("mom!") + 1).append("dad!");
    test = std::move(test.resize(test.length()).append('\0'));
    printf("{resize_test}  test:     \"%s\"\n", test.data());
    printf("{resize_test}  length:    %zu\n", test.length());
    printf("{resize_test}  capacity:  %zu\n", test.capacity());
    printf("{resize_test}  last char: %i\n", test[test.length()]);
}

void dystring_foreach_test() {
    crim::string test = "Hi mom!";
    // Does not include nul char as `.end()` points to it most likely.
    for (const auto &c : test) {
        printf("%c (%i)\n", c, c);
    }
    printf("\n");

    crim::dyarray<int> list = {1, 2, 3, 4, 5};
    list.push_back(7);
    // Should be false regardless if we push back or not, and it is!
    printf("(list) empty?: %s\n", tostring(list.empty()));
    for (const auto &i : list) {
        printf("%i, ", i);
    }
    printf("\n");

    crim::dyarray<int> none = {};
    // none.push_back(14);
    // When above line is commented out, this is true as expected! :)
    printf("(none) empty?: %s\n", tostring(none.empty()));
    for (const auto &i : none) {
        printf("%i, ", i);
    }
    printf("\n");
}

void dystring_copyctor_test() {
    crim::string shorten = "Shorten me!";
    printf("{shorten}: \"%s\"\n", shorten.data());
    // Before, this would mess up. Now with the move-constructor this is ok.
    shorten = shorten
        .resize(shorten.length() - sizeof("me!"))
        .append("ed!!!");
    printf("{resized}: \"%s\"\n", shorten.data());
    // Since crim::base_dyarray has the needed for each iterator functions
    // `.begin()` and `.end()`, we can do this nicely!
    for (const auto &c : shorten) {
        printf("%c (%i)\n", c, c);
    }
}

void dystring_movector_test() {
    // Seems that this calls the basic constructor.
    crim::string test = crim::string("Hi mom!");
    printf("{test}: \"%s\"\n", test.data());

    // Calls move-assignment operator.
    test = crim::string("Hello there!");

    // Would result in a garbage buffer without the move constructor.
    test = test;
    printf("{test}: \"%s\"\n", test.data());

    // Make sure our inherited [] operator works!
    for (size_t i = 0; i < test.length(); i++) {
        printf("%c (%i)\n", test[i], test[i]);
    }
}

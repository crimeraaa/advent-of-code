#include <stdio.h>

#include <functional>

#include <crim/dyarray.tcc>
#include <crim/dystring.tcc>

void crim_print_action(
    const char *whence,
    const char *action, 
    const char *s, 
    const char *t = "(null)"
) {
    printf("%s %s: {s} = \"%s\"\n", whence, action, s);
    printf("%s %s: {t} = \"%s\"\n\n", whence, action, t);
}

// Try to force a move-assignment.
void crim_string_move_test() {
    crim::string s = "hi mom!";
    crim_print_action("before", "move", s.c_str());
    crim::string t = std::move(s);
    crim_print_action("before", "move", s.c_str(), t.c_str());
}

// Try to force a copy-assignment.
void crim_string_copy_test() {
    crim::string s = "hi mom!";
    crim_print_action("before", "copy", s.c_str());
    crim::string t = s;
    crim_print_action("after", "copy", s.c_str());
    crim_print_action("after", "copy", t.c_str());
}

void crim_dyarray_int_test() {
    crim::dyarray<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    v.push_back(16).push_back(17).push_back(18).push_back(19).pop_back();
    // for (const auto &i : v) {
    //     printf("%i\n", i);
    // }
    for (const auto *p = v.begin(); p < v.end(); p++) {
        printf("v[%td]: %i (%p)\n", p - v.begin(), *p, (void*)p);
    }
}

void crim_dyarray_string_test() {
    crim::dyarray<crim::string> v;
    v.push_back("Hi mom!").push_back("Hello there!").push_back("I like cats!");
    // popped back elements are automatically destroyed!
    v.push_back("Nope!").pop_back();
    for (const auto &s : v) {
        printf("\"%s\"\n", s.c_str());
    }
    for (const auto *p = v.begin(); p < v.end(); p++) {
        printf("v[%td]: \"%s\" (%p)\n", p - v.begin(), p->c_str(), (void*)p);
    }
}

int main() {
    // crim_string_move_test();
    // crim_string_copy_test();
    // crim_dyarray_int_test();
    crim_dyarray_string_test();
    return 0;
}

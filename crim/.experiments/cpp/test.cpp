#include "c-style.hpp"

void ga_int_test() {
    // This relies on you to be 100% certain of the type!!!!
    generic_array ga_int; ga_int.init(sizeof(int));
    for (int i = 0; i < 16; i++) {
        ga_int.push_back(&i);
    }

    // This whole loop should be OK
    for (size_t i = 0; i < ga_int.length(); i++) {
        int x; ga_int.at(i, &x);
        printf("ga_int[%llu]: %i\n", i, x);        
    }

    // This should print an error message
    int x; ga_int.at(ga_int.length(), &x);
    
    // This should be OK
    ga_int.deinit();
}

const char *g_messages[] = {
    "Hi mom!",
    "Hello there.",
    "The quick brown fox jumps over the lazy dog",
    "She sells sea shells by the sea shore, but the value of these shells...",
};

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

void ga_readonly_string_test() {
    // 1D array of pointers, each of which points to an address in g_messages.
    generic_array ga_str; ga_str.init(sizeof(const char**));

    // Copy the address of the read-only string literals.
    for (size_t i = 0, len = ARRAY_LENGTH(g_messages); i < len; i++) {
        ga_str.push_back(&g_messages[i]);
    }

    // We should have the exact same pointers as g_messages. 
    for (size_t i = 0; i < ga_str.length(); i++) {
        const char *s; ga_str.at(i, &s);
        printf("ga_str[%llu] = %s\n", i, s);
    }
    ga_str.deinit();
}

#define STRINGLENGTH 256

static void copy_string(void *p_dest, const void *p_src) {
    
}

void ga_writeable_string_test() {
    generic_array ga_str; ga_str.init(sizeof(char*));
    char p_buffer[STRINGLENGTH];
    for (size_t i = 0, len = ARRAY_LENGTH(g_messages); i < len; i++) {
        strncpy(p_buffer, g_messages[i], sizeof(p_buffer));        
    }
}

int main() {
    // ga_int_test();
    ga_readonly_string_test();
    return 0;
}

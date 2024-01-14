#include "optimize.hpp"

void cs_test(const cstring &inst) {
    printf("\"%s\"\n", inst.data());
    for (size_t i = 0; i < inst.length(); i++) {
        char ch = inst.at(i);
        // Dr. Memory says the %#x spec reads 8 bytes beyond top of stack...
        printf("'%c' = %#x\n", ch, ch);
    }
    printf("\n");
}

// TODO: Make more robust
int main() {
    cstring cs = "Hi mom!";
    cstring cs2 = "This is a very long string that should cause allocations.";
    (void) cs; cs_test(cs);
    (void) cs2; cs_test(cs2);
    return 0;
}

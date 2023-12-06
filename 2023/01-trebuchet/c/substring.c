#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// @returns Pointer to start of section that contains the substring.
// @warning Casts away `const` qualifier. Good luck!
// @note Only works for nul terminated strings. Good luck!
char *get_substring(const char *src, const char *sub) {
    // Find first occurence of substring's first char in source
    src = strchr(src, sub[0]);
    if (src == NULL || src[0] == '\0') {
        return NULL; // No match of first substring char or got empty string.
    }

    const char *copy = src; // Hold copy to return `src` as is
    char ch_src = *copy; // store in value to lessen ptr dereference
    char ch_sub = *sub; 

    // Peek ahead for matches
    while (ch_src != '\0' && ch_sub != '\0' && ch_src == ch_sub) {
        ch_src = *(copy++); // postfix operand returns original value before 
        ch_sub = *(sub++); // incrementing the value stared by this variable.
    }

    // Exhausted substring, assume successfully matched the whole substring.
    if (ch_sub == '\0') {
        return (char*)src; // strchr returned ptr to substring start already.
    }
    return NULL; // Implied else, too bad!
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("---- Find a substring! ----\n");
        printf("Usage: %s <source> <substr>\n", argv[0]);
        return 1;
    }
    const char *source = argv[1];
    const char *substr = argv[2];
    const char *match = get_substring(source, substr);
    printf("source: \"%s\"\nsubstring: \"%s\"\n", source, substr);
    if (match == NULL) {
        printf("No matches of substring found.\n");
    } else {
        printf("get_substring: \"%s\"\n", match);
    }
    return 0;
}

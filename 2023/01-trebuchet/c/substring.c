#include <stdio.h>
#include <stdlib.h>

// @returns Pointer to start of section that contains the substring.
// @note Only works for nul terminated strings. Good luck!
char *get_substring(const char *str, const char *sub) {
    while (*str != '\0') {
        const char *str1 = str; // start w/ current main letter being checked
        const char *sub1 = sub; // locally scoped, need main `sub` ptr throughout

        // Peek ahead for matching characters in both strings
        while (*str1 == *sub1 &&  *str1 != '\0' && *sub1 != '\0') {
            str1++;
            sub1++;
        }
        // Exhausted the substring, probably means we found it!
        if (*sub1 == '\0') {
            return str;
        } 
        str++; // move on to next letter
    }
    return NULL;
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
    printf("source: \"%s\", substr: \"%s\"\n", source, substr);
    if (match == NULL) {
        printf("No matches for substring \"%s\".\n", substr);
    } else {
        printf("get_substring: \"%s\"\n", match);
    }
    return 0;
}

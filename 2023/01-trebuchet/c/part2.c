#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pray none of the Advent of Code inputs exceeds this :)
#define BUFFERSIZE 256

// Won't work for decayed pointers!
#define ARRAYLENGTH(x) (sizeof(x) / sizeof(x[0]))

#define eprintf(msg) fprintf(stderr, __FILE__ ":%i: " msg "\n", __LINE__)

const char *SPELLINGS[] = {
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};

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
            return (char*)str; // discard const but prolly a bad idea :)
        } 
        str++; // move on to next letter
    }
    return NULL;
}

int match_spelling(const char *buffer) {
    for (size_t i = 0; i < ARRAYLENGTH(SPELLINGS); i++) {
        const char *spelling = SPELLINGS[i];
        // Check if `buffer` contains substring `spelling`. Totally safe!
        // char *match = strstr(buffer, spelling);
        if (get_substring(buffer, spelling) != NULL) {
            // Add 1 to 0-based index to get value represented by this word
            return i + 1;
        }
    }
    // Fallback to 0 (also considered false in C/C++)
    return 0;
}

// No bounds checking. Good luck!
struct List {
    int array[BUFFERSIZE];
    int index;
    int *end; // hold a pointer to the last nonzero element for later
};

// pls optimize kthxbai (dereferences pointer alot so maybe a lil slow)
static inline void update_list(struct List *plist, int value) {
    plist->array[plist->index] = value;
    plist->end = &plist->array[plist->index];
    plist->index++;
}

// Get the leftmost and rightmost numbers represented as digits or strings.
// Builds substring and constantly compares to SPELLINGS array.
int match_numbers(const char *buffer, size_t length) {
    struct List list = {0};
    char writer[BUFFERSIZE] = {0}; // stack-allocated cuz I am lazy as hell
    int index = 0; // pray we don't buffer overflow LMAO
    list.end = &list.array[0]; // just so its never NULL
    list.index = 0;
    for (size_t i = 0; i < length; i++) {
        char c = buffer[i];
        if (isdigit(c)) {
            // Adjust for ASCII encoding, need the actual value not char
            update_list(&list, c - '0');
            continue;
        }
        // Build substring char by char, index++ return prev. val. b4 incr.
        writer[index++] = c;
        // If got match, prep for reset
        int matched = match_spelling(writer);
        if (matched) {
            update_list(&list, matched);
            // Clear string so we don't use old information on next checks
            memset(writer, 0, sizeof(writer)); 
            index = 0;
            // Start string at last char, e.g. for `twone`, `oneight`, etc.
            writer[index++] = c;
        }
    }
    return (list.array[0] * 10) + *list.end;
}

int main(int argc, char *argv[]) {
    const char *name = (argc == 2) ? argv[1] : "../part2.txt";
    FILE *file = fopen(name, "r");
    if (!file) {
        eprintf("Failed to open file!");
        return 1;
    }
    int count = 1; // Current line number
    int sum = 0;
    char buffer[BUFFERSIZE] = {0}; // Pray we don't overflow this :)

    // Read file linewise (fgets will return NULL on EOF), incr. file dataptr
    while (fgets(buffer, BUFFERSIZE, file) != NULL) {
        // strcspn gets index of the first occurence of any char in 2nd arg
        buffer[strcspn(buffer, "\r\n")] = '\0';
        int matched = match_numbers(buffer, strlen(buffer));
        printf("%i: %s\t%i\n", count++, buffer, matched);
        sum += matched;
    }
    printf("Calibration Value: %i\n", sum);
    fclose(file);
    return 0;
}

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 256 // totally safe!

#define eprintf(msg) fprintf(stderr, __FILE__ ":%i: " msg "\n", __LINE__)

// Totally safe!
int find_number(const char *buffer, int increment) {
    char c = -1;
    while (!isdigit(c)) {
        c = *buffer;
        buffer += increment;
    }
    // If didn't find a number, default to 0
    return (c > 0) ? c - '0' : 0;
}

// Gets the first and last digit of `buffer`
int match_numbers(const char *buffer, const int length) {
    // Avoid working with empty strings
    if (*buffer == '\0') {
        return 0;
    }
    // From start of string, try to find a number to the right
    int x = find_number(buffer, 1);
    // Get pointer to end of string, try find a number to the left 
    int y = find_number(buffer + length, -1);
    return (x * 10) + y;
}

int main(int argc, char *argv[]) {
    // totally safe!
    const char *name = (argc == 2) ? argv[1] : "../part1.txt";
    FILE *file = fopen(name, "r");
    if (!file) {
        eprintf("Failed to open file!");
        return 1;
    }
    // Pray we don't overflow this :)
    char buffer[BUFFERSIZE] = {0};
    int count = 1;
    int sum = 0;
    // Return NULL on EOF
    while (fgets(buffer, BUFFERSIZE, file) != NULL) {
        // Get index of first occurence of some newline char
        buffer[strcspn(buffer, "\r\n")] = '\0';
        int matched = match_numbers(buffer, strlen(buffer));
        printf("%i: %s\t%i\n", count++, buffer, matched);
        sum += matched;
    }
    printf("Calibration Value: %i\n", sum);
    fclose(file);
    return 0;
}

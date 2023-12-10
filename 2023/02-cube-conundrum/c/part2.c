#include <ctype.h> // isdigit
#include <stdbool.h>
#include <stddef.h> // ptrdiff_t
#include <stdlib.h> // malloc family, NULL
#include <stdio.h> // printf family, FILE*, f_ family
#include <string.h> // str_ family

// Header-only cause I cannot be bothered to deal with translatoin units today
#define CRI_IO_HELPERS_IMPL 1
#include "helpers.h"

enum CUBE_COLOR_INFO {
    CUBE_COLOR_RED, 
    CUBE_COLOR_GREEN, 
    CUBE_COLOR_BLUE, 
    // CUBE_COLOR_COUNT,
    // CUBE_COLOR_INVALID
};

struct Cube {
    int count;
    enum CUBE_COLOR_INFO color; // Use enum CUBE_COLOR_INFO
};

struct Set {
    struct Cube red, green, blue;
};

// Hardcoded cause VSCode debugger uses workspace as CWD
#ifdef _WIN32
#define FALLBACK_DIRECTORY \
"C:/Users/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/"
#else
// Remember that Linux has a vastly different file system.
#define FALLBACK_DIRECTORY \
"/home/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/"
#endif

// Determines the count and color of this cube in the set.
struct Cube game_get_cube(const char *elem_start, const char *elem_end)
{
    struct Cube cube = {0};
    const char *elem = elem_start;
    char c;
    while (elem < elem_end && (c = *elem) && isdigit(c)) {
        cube.count = (cube.count * 10) + c - '0';
        elem++;
    }
    const char *color = strchr(elem, ' ') + 1;
    ptrdiff_t length = elem_end - color;
    printf("%i %.*s, ", cube.count, (int)length, color); // ! DEBUG

    // Since color points to the remainder of the original string,
    // limit the comparison via our substring's determined length.
    if (strncmp("red", color, length) == 0) {
        cube.color = CUBE_COLOR_RED;
    } else if (strncmp("green", color, length) == 0) {
        cube.color = CUBE_COLOR_GREEN;
    } else if (strncmp("blue", color, length) == 0) {
        cube.color = CUBE_COLOR_BLUE;
    }
    return cube;
}

// Tallies the current semicolon separated set.
struct Set game_get_set(int setno, const char *sets, const char *semi)
{
    struct Set current = {0};
    const char *comma = NULL;
    printf("Set %i: ", setno); // ! DEBUG
    while (comma < semi) {
        comma = strchr(sets, ','); // point to char after comma
        // `sets` still points most of the original string so clip `comma`!
        if (comma == NULL || comma >= semi) {
            comma = semi;
        }
        struct Cube cube = game_get_cube(sets, comma);
        switch (cube.color)
        {
            case CUBE_COLOR_RED: {
                current.red = cube;
                break;
            }
            case CUBE_COLOR_GREEN: {
                current.green = cube;
                break;
            }
            case CUBE_COLOR_BLUE: {
                current.blue = cube;
                break;
            }
        }
        sets = strchr(comma, ' ') + 1;
    }
    printf("\n"); // ! DEBUG
    return current;
}

// Determines the minimum number of cubes per color needed for this game.
int game_get_power(const char *sets, const char *endl)
{
    struct Set total = {0};
    const char *semi = NULL;
    int setno = 1;
    while ((semi < endl) && (sets != NULL)) {
        semi = strchr(sets, ';'); // points to exact position in string
        if (semi == NULL) {
            semi = endl;
        }
        // ptrdiff_t setlen = semi - set; // Substr. length w/o semi

        // Split the current set into its cube counts.
        struct Set set = game_get_set(setno, sets, semi);
        // Update all 3 separately
        if (set.red.count > total.red.count) {
            total.red.count = set.red.count;
        }
        if (set.green.count > total.green.count) {
            total.green.count = set.green.count;
        }
        if (set.blue.count > total.blue.count) {
            total.blue.count = set.blue.count;
        }

        // Find whitespace after semi and point to char past that
        sets = strchr(semi + 1, ' ') + 1; // point to start of next element
        setno++;
    }
    printf("\n"); // ! DEBUG
    return total.red.count * total.green.count * total.blue.count;
}

int game_get_sum(char **buffer, int length)
{
    int sum = 0;
    for (int i = 0; i < length; i++) {
        char *line = buffer[i];
        const char *endl = strchr(line, '\0');
        const char *colon = strchr(line, ':');
        const char *sets = strchr(colon, ' ') + 1; // ptr to very first set's char
        int game_id = 0;
        // ptrdiff_t gameinfo_len = colon - line; // Substr. length w/o colon
        sscanf(line, "Game %d:", &game_id);
        printf("Game %d : \"%s\"\n", game_id, sets);
        sum += game_get_power(sets, endl);
    }
    return sum;
}

int main(int argc, char *argv[])
{
    int exitcode = EXIT_SUCCESS;
    if (argc != 1 && argc != 2) {
        printf("Usage: %s [inputfile]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = (argc == 2) ? argv[1] : FALLBACK_DIRECTORY "sample.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        eprintf("Failed to open input file.");
        return EXIT_FAILURE + 1;
    }

    struct StrVector *lines = cri_strvec_new();
    if (lines == NULL) {
        exitcode = EXIT_FAILURE + 2;
        goto FILE_CLEANUP;
    }
    
    if (!cri_readfile(file, lines)) {
        exitcode = EXIT_FAILURE + 3;
        goto VECTOR_CLEANUP;
    }

    printf("<SUM>: %i\n", game_get_sum(lines->buffer, lines->index));

    // Not needed for toy programs, but it's good to make it a habit!)
    VECTOR_CLEANUP:
    cri_strvec_delete(lines);
    FILE_CLEANUP:
    fclose(file);    
    return exitcode;
}

#include <ctype.h> // isdigit
#include <stdbool.h>
#include <stddef.h> // ptrdiff_t
#include <stdlib.h> // malloc family, NULL
#include <stdio.h> // printf family, FILE*, f_ family
#include <string.h> // str_ family

// Header-only cause I cannot be bothered to deal with translatoin units today
#define CRI_IO_HELPERS_IMPL 1
#include "helpers.h"

#define ELF_CRITERIA_RED    12
#define ELF_CRITERIA_GREEN  13
#define ELF_CRITERIA_BLUE   14

// Hardcoded cause VSCode debugger uses workspace as CWD
#ifdef _WIN32
#define FALLBACK_DIRECTORY \
"C:/Users/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/"
#else
// Remember that Linux has a vastly different file system.
#define FALLBACK_DIRECTORY \
"/home/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/"
#endif

bool game_elem_result(const char *elem_start, const char *elem_end)
{
    const char *elem = elem_start;
    int count = 0;
    char c;
    while (elem < elem_end && (c = *elem) && isdigit(c)) {
        count = (count * 10) + c - '0';
        elem++;
    }
    const char *color = strchr(elem, ' ') + 1;
    ptrdiff_t length = elem_end - color;
    // ! DEBUG
    printf("count = %i, color = \"%.*s\"\n", count, (int)length, color);

    // Since color points to the remainder of the original string,
    // limit the comparison via our substring's determined length.
    if (strncmp("red", color, length) == 0 && count > ELF_CRITERIA_RED) {
        return false;
    } else if (strncmp("green", color, length) == 0 && count > ELF_CRITERIA_GREEN) {
        return false;
    } else if (strncmp("blue", color, length) == 0 && count > ELF_CRITERIA_BLUE) {
        return false;
    }
    return true;
}

bool game_split_elems(int setno, const char *sets, const char *semi)
{
    int count = 1;
    const char *comma = NULL;
    while (comma < semi) {
        // ! DEBUG
        printf("set@%i:%i: ", setno, count++);
        comma = strchr(sets, ','); // point to char after comma
        // `sets` still points most of the original string so clip `comma`!
        if (comma == NULL || comma >= semi) {
            comma = semi;
        }
        // Need to evaluate separately so function call is guaranteed
        if (!game_elem_result(sets, comma)) {
            return false;
        }
        sets = strchr(comma, ' ') + 1;
    }
    return true;
}

bool game_split_sets(const char *sets, const char *endl)
{
    const char *semi = NULL;
    int setno = 1;
    while ((semi < endl) && (sets != NULL)) {
        semi = strchr(sets, ';'); // points to exact position in string
        if (semi == NULL) {
            semi = endl;
        }
        // ptrdiff_t setlen = semi - set; // Substr. length w/o semi
        if (!game_split_elems(setno, sets, semi)) {
            return false;
        }

        // Find whitespace after semi and point to char past that
        sets = strchr(semi + 1, ' ') + 1; // point to start of next element
        setno++;
    }
    return true;
}

// Tests if the given game matches the elf's criteria.
// @param line String containing this game's sets, elements and results.
// @return Game ID if matched the criteria, else 0.
int game_fits_criteria(const char *line) 
{
    const char *endl = strchr(line, '\0');
    const char *colon = strchr(line, ':');
    const char *sets = strchr(colon, ' ') + 1; // ptr to very first set's char
    int game_id = 0;
    // ptrdiff_t gameinfo_len = colon - line; // Substr. length w/o colon
    sscanf(line, "Game %d:", &game_id);
    printf("Game %d : \"%s\"\n", game_id, sets);
    bool is_possible = game_split_sets(sets, endl);
    // ! DEBUG
    if (game_split_sets(sets, endl)) {
        printf("<VALID>: This game met the elf's criteria!\n");
    } else {
        printf("<INVALID>: This game exceeded 1/more of the elf's criteria!\n");
    }
    printf("\n");
    return is_possible ? game_id : 0;
}

int main(int argc, char *argv[])
{
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

    struct StrVector *lines = cri_strvec_new(file);
    if (lines == NULL) {
        fclose(file);
        eprintf("Failed allocate memory for lines vector.");
        return EXIT_FAILURE + 2;
    }

    int sum = 0;
    for (int i = 0; i < lines->index; i++) {
        char *line = lines->buffer[i];
        sum += game_fits_criteria(line);
    }
    printf("<SUM>: %i\n", sum);

    // Not needed for toy programs, but it's good to make it a habit!)
    cri_strvec_delete(lines);
    fclose(file);
    return EXIT_SUCCESS;
}

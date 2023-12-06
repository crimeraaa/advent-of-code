#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

using namespace std; // I cannot be bothered to deal with C++'s crap today

#define eprintf(msg) fprintf(stderr, __FILE__ ":%i: " msg "\n", __LINE__)

enum CUBE_ID {CUBE_RED, CUBE_GREEN, CUBE_BLUE, CUBE_COUNT};

struct Cube {
    enum CUBE_ID color; // one of "red", "blue" or "green"
    int amount; // nonzero?
};

// No need struct, + we're only concerned w/ 3 cubes.
using Set = array<Cube, CUBE_COUNT>;

struct Game {
    int id; // Game number, e.g. Game 1 or Game 2 or Game 11
    vector<Set> sets; // Elf will "shuffle" an unknown amount of times
};

void tokenize_to_game(const string &buffer) {
    Game game;
    game.id = 0;
    size_t colon = buffer.find(':'); // match "Game <number>:"
    string test = buffer.substr(0, colon);
    printf("\tTest: \"%s\"\n", test.c_str());
    for (const auto &c : test) {
        if (isdigit(c)) {
            game.id = (game.id * 10) + c - '0';
        }
    }
    printf("\tGame ID: %i\n", game.id);
    size_t semi = buffer.find(';'); // semicolon
    test = buffer.substr(colon + 1, semi - test.length());
    printf("\tSet 1: \"%s\"\n", test.c_str());
}

int main(int argc, char *argv[]) {
    // Game numbers, comma separated elems, semicolon separated sets
    ifstream file((argc == 2) ? argv[1] : "../part1.txt");
    if (!file.is_open()) {
        eprintf("Failed to open input file!");
        return 1;
    }
    int count = 1; // file line number
    string buffer; // a line ends at first newline char
    // Fill buffer w/ contents of line, sans newline
    while (getline(file, buffer)) {
        printf("%i : %s\n", count++, buffer.c_str());
        tokenize_to_game(buffer);
    }
    file.close();
    return 0;
}

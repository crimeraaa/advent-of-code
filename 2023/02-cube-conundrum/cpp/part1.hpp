#pragma once
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#define eprintf(msg) std::fprintf(stderr, __FILE__ ":%i: " msg "\n", __LINE__)

// Use behaviour of enums to our advantage to determine the count
enum class CUBE_ID {RED, GREEN, BLUE, COUNT};

struct Cube {
    std::string color = "(empty)"; // one of "red", "blue" or "green"
    int count = 0; // default ot 0 so we don't get garbage
};

// Results of a bunch of cubes
struct Set {
private:
    // Each set only ever has 3 cubes
    std::array<Cube, static_cast<size_t>(CUBE_ID::COUNT)> m_buffer;
public:
    // Read and write access to the buffer for convinience
    Cube &operator[](CUBE_ID key) {
        return m_buffer[static_cast<size_t>(key)]; // lol this is ugly as hell
    };
};

struct Game {
    int id; // integral part of `"Game 1"`, `"Game 2"`, `"Game 11"`, etc.
    std::vector<Set> sets; // Elf will "shuffle" an unknown amount of times
};

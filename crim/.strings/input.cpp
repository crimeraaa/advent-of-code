#include <cstdio>

#include "input.hpp"

int main() {
    auto s = crim::get_string("Enter your name: ");
    std::printf("Hi %s! (count: %zu, capacity: %zu)\n", s.c_str(), s.length(), s.capacity());
    return 0;
}

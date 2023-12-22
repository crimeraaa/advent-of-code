#include "dyarray.hpp"
#include "string.hpp"
#include "io.hpp"

#ifdef _WIN32
#define FALLBACK "C:/Users/crimeraaa/repos/advent-of-code/crim/test.txt"
#else
#define FALLBACK "/usr/crimeraaa/repos/advent-of-code/crim/test.txt"
#endif

int main(int argc, char *argv[]) {
    auto lines = crim::readfile((argc == 2) ? argv[1] : FALLBACK);
    for (size_t i = 0; i < lines.size(); i++) {
        auto &line = lines.at(i);
        printf("%s\n", line.c_str());
    }
    // lines.destroy();
    return 0;
}

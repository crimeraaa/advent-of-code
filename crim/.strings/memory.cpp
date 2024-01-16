#if defined(__MINGW32__) || defined(__MINGW64__)
#define __USE_MINGW_ANSI_STDIO_1
#endif

#include <cstdio>
#include <cmath>
#include <climits>
#include "memory.tcc"

template<class T> static constexpr int get_bits() noexcept
{
    return sizeof(T) * CHAR_BIT;
}

void list_powers_of_2()
{
    static constexpr int max_bits = get_bits<int>();
    for (int i = 0; i <= max_bits; i++) {
        std::printf("2^%i = %f\n", i, std::pow(2, i));
    }
}

int main()
{
    list_powers_of_2();
    return 0;
}

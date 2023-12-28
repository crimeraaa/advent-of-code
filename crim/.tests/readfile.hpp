#pragma once

#ifdef _WIN32
#define FALLBACK "C:/users/crimeraaa/repos/advent-of-code/crim/.tests/"
#else
#define FALLBACK "/home/crimeraaa/repos/advent-of-code/crim/.tests/"
#endif

#define STD_READFILE_USE_FILEPTR

template<typename IntT> int get_digits(IntT value) {
    int digits = 0;
    if constexpr(std::is_signed<IntT>::value) {
        value *= (value >= 0) ? 1 : -1;
    }
    while (value > 0) {
        digits++;
        value /= 10;
    }
    return digits;
}

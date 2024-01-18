#include <cstdio>
#include <cstring>
#include <cwchar>
#include <algorithm>

#include "type_traits.tcc"

static constexpr size_t bufsize = 256;

#define array_length(array) (sizeof(array) / sizeof(*array))

void compare_wstrings(const wchar_t *s1, const wchar_t *s2, std::size_t len)
{
    using T = wchar_t;
    using traits = crim::char_traits<T>;

    int result = traits::compare(s1, s2, len);
    // Seems %s still looks for normal char* even with std::wprintf.
    std::printf("s1: \"%ls\"\n", s1);
    std::printf("s2: \"%ls\"\n", s2);
    std::printf("result: %i\n", result);
    if (result > 0) {
        std::printf("s1 is greater than s2!\n");
    } else if (result < 0) {
        std::printf("s1 is less than s2!\n");
    } else {
        std::printf("s1 is equal to s2!\n"); 
    }
}

int main() 
{
    wchar_t s1[] = L"Hi mom!";
    wchar_t s2[bufsize] = {0};
    constexpr size_t len = array_length(s1); (void)len;
    constexpr size_t size = sizeof(*s1) * len; (void)size;
    crim::char_traits<wchar_t>::copy(s2, s1, len);
    compare_wstrings(s1, s2, len);
    return 0;
}

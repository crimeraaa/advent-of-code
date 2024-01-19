#include <cstdio>
#include <cwchar>

#include "../base_string.tcc"

void print_wcstring(const crim::wcstring &s)
{
    std::printf("\"%ls\" (length: %zu, capacity: %zu)\n", s.c_str(), s.length(), s.capacity());
}

void print_move()
{
    crim::wcstring a{L"The quick brown fox jumps over the lazy dog."};
    crim::wcstring b{crim::rvalue_cast(a)};
    print_wcstring(a);
    print_wcstring(b);//
}

int main()
{
    crim::wcstring s = L"Hi mom!";
    print_wcstring(s);
    print_move();
    return 0;
}

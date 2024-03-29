#include "type_traits.tcc"
#include "utility.tcc"
#include "../base_string.tcc"
#include <cstdio>

void test_lvalue_reference(const int &lvalue)
{
    using T = decltype(lvalue);
    // We're extracting the raw type and calling the copy-ctor.
    auto x = crim::remove_reference<T>::type{lvalue};
    auto y = crim::remove_reference_t<T>{lvalue};
    printf("Hello lvalue %i!\n", lvalue);
    printf("Hello auto x = %i!\n", x);
    printf("Hello auto y = %i!\n", y);
}

void test_rvalue_reference(const int &&rvalue)
{
    using T = decltype(rvalue);
    // We're extracting the raw type and calling the move-ctor.
    auto x = crim::remove_reference<T>::type{rvalue};
    auto y = crim::remove_reference_t<T>{rvalue};
    printf("Hello rvalue %i!\n", rvalue);
    printf("Hello auto x = %i!\n", x);
    printf("Hello auto y = %i!\n", y);
}

void test_cstring()
{
    crim::cstring s = "Hi mom!";
    crim::cstring s2 = "The quick brown fox jumps over the lazy dog.";
    printf("s: %s (length: %zu, capacity: %zu)\n", s.c_str(), s.length(), s.capacity());
    printf("s2: %s (length: %zu, capacity: %zu)\n", s2.c_str(), s2.length(), s2.capacity());
}

int main()
{
    int x = 13;
    test_lvalue_reference(x); // Should be a compile-time error!
    test_rvalue_reference(crim::rvalue_cast(x)); // Should be valid!
    test_rvalue_reference(101); // literals usually go for rvalue references
    test_cstring();
    return 0;
}

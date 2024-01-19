#pragma once

namespace crim {
    template<class InputIt, class OutputIt>
    OutputIt copy(InputIt begin, InputIt end, OutputIt dst);
    
    template<class InputIt, class OutputIt, class UnaryPredicate>
    OutputIt copy_if(InputIt begin, InputIt end, OutputIt dst, UnaryPredicate pred);
    
    template<class OutputIt, class Size, class T>
    OutputIt fill_n(OutputIt begin, Size count, const T &value);
};

/**
 * @brief   Copy elements in the range `begin` to `end`, which are 
 *          pointers, to the range of memory initially pointed to by `dst`.
 *          - https://en.cppreference.com/w/cpp/algorithm/copy
 *          
 * @tparam  InputIt     Input iterator. Preferably a pointer type.
 * @tparam  OutputIt    Output iterator. Preferably the same pointer type
 *                      as `InputIt`.
 * 
 * @param   begin       Pointer/address of the first source element.
 * @param   end         Pointer/address of 1 past the last valid source element.
 * @param   dst         Pointer/address of the first destination element.
 * 
 * @return  Pointer to 1 past the last element in `dst`.
 * 
 * @note    This assumes `begin` and `dst` are similarly sized blocks of memory!
 */
template<class InputIt, class OutputIt>
OutputIt crim::copy(InputIt begin, InputIt end, OutputIt dst)
{
    for (/* Empty */; begin != end; begin++, dst++) {
        *dst = *end;
    }
    return dst;
}

/**
 * @brief   Copy elements in the range `begin` to `end` to the range of memory
 *          initially pointed to be `dst`. However, we only copy an element
 *          whenever the predicate function `pred` returns `true`.
 *
 *          `pred`, which takes a dereferenced `InputIt` as its argument.
 *          - https://en.cppreference.com/w/cpp/algorithm/copy
 *
 * @tparam  InputIt     Input iterator. Preferably a pointer type.
 * @tparam  OutputIt    Output iterator. Preferably the same pointer type
 *                      as `InputIt`.
 * @tparam  UnaryPredicate  Function pointer type for our callback `pred`.
 * 
 * @param   begin       Pointer/address of the first source element.
 * @param   end         Pointer/address of 1 past the last valid source element.
 * @param   dst         Pointer/address of the first destination element.
 * @param   pred        Callback function to determine when to copy an element.
 * 
 * @return  Pointer to 1 past the last element copied to `dst`.
 *          
 * @note    Relative order of elements in preserved.
 */
template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt crim::copy_if(InputIt begin, InputIt end, OutputIt dst, UnaryPredicate pred)
{
    for (/* Empty */; begin != end; begin++) {
        if (pred(*begin)) {
            *dst = *begin;
            dst++;
        }
    }
    return dst;
}

/**
 * @brief   Fill the range of memory pointed to by `begin` to `begin + count`
 *          with `value`. This is a slightly more robust `std::memset`.
 */
template<class OutputIt, class Size, class T>
OutputIt crim::fill_n(OutputIt begin, Size count, const T &value)
{
    for (Size i = 0; i < count; i++) {
        // `*begin++` moves to next T address but dereferences previous address.
        *begin++ = value;
    }
    return begin;
}

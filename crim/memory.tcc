#pragma once

#include <cstdlib> /* std::malloc, std::free */
#include <stdexcept> /* std::out_of_range */
#include <memory> /* std::pointer_traits */
#include <new> /* std::bad_array_new_length, std::bad_alloc */

#include "logerror.hpp"

namespace crim {
    template<class T> 
    constexpr bool usage_assert(bool b_usage) noexcept;

    template<class T> 
    struct allocator;

    template<class Alloc> 
    struct allocator_traits;
};

/**
 * @brief   Mainly used in `static_assert` of `crim::allocator_traits`.
 *          It's to prevent instantiations while keeping the template around.
 * 
 * @note    With `static_assert(false, "")` the template definition just fails. 
 */
template<class T> 
constexpr bool crim::usage_assert(bool b_usage) noexcept
{
    return b_usage;
}

#define crim_logerror(func, info) \
    crim_logerror_nofunc("crim::allocator<T>", func, info)

/**
 * Custom allocator to be used by `std::allocator_traits`, or just use it
 * manually. Your choice.
 *  - https://learn.microsoft.com/en-us/cpp/standard-library/allocators?view=msvc-170#writing-your-own-allocator-c11k
 */
template<class T> 
struct crim::allocator {
    // Need these for use w/ std::allocator traits.
    using value_type = T;
    using size_type = std::size_t;
    
    enum limit : size_type {
        limit_lower = 1,
        limit_upper = static_cast<size_type>(-1) / sizeof(value_type)
    };

    // Default constructor is not required by the C++ standard library.
    allocator() noexcept
    {}

    // Converting copy-constructor.
    template<class OtherT> 
    allocator(const allocator<OtherT> &other) noexcept
    {}
    
    static constexpr size_type max_size() noexcept 
    {
        return limit_upper;
    }

    // Prevent users from requesting a size of 0 or something ridiculously big.
    static constexpr bool is_valid_size(size_type n_count) noexcept
    {
        return n_count >= limit_lower && n_count < limit_upper;
    }

    /**  
     * @brief   Validates given size before trying to allocate a memory block.
     *
     * @param   n_count     How many ELEMENTS, not BYTES, are desired.
     * 
     * @exception   `std::bad_array_new_length()`, `std::bad_alloc()`.
    */
    T *allocate(size_type n_count) const
    {
        if (!is_valid_size(n_count)) {
            // This may be a bad idea, std::allocator doesn't do this.
            if (n_count == 0) {
                return nullptr; 
            }
            crim_logerror("allocate", "Requested too much memory!");
            throw std::bad_array_new_length();
        }
        // Running out of memory IS an exceptional situation.
        //  - https://stackoverflow.com/a/4827445
        void *p_memory{std::malloc(sizeof(T) * n_count)};
        if (p_memory == nullptr) {
            crim_logerror("allocate", "Failed to allocate memory!");
            throw std::bad_alloc();
        }
        return static_cast<T*>(p_memory); 
    }
    
    /**
     * @brief       This literally just frees the pointer.
     *
     * @note        Could destroy all elements here but I'd prefer containers 
     *              explicitly do it themselves as to give them full control.
    */
    void deallocate(T *p_memory, size_type n_count) const noexcept
    {
        (void)n_count; // Signature needed by std::allocator, but we don't need.
        std::free(p_memory);
    } 
};

#undef crim_logerror

/**
 * @brief   See: https://en.cppreference.com/w/cpp/memory/allocator_traits
 * @tparam  Alloc   A template instantiation of an allocator, be it `std` or `crim`.
 */
template<class Alloc> struct crim::allocator_traits {
    // static_assert(crim::usage_assert<Alloc>(false), "Don't use this!");

    using allocator_type     = Alloc;
    using value_type         = typename allocator_type::value_type;
    using size_type          = typename allocator_type::size_type;
    
    using pointer            = value_type*;
    using pointer_traits     = std::pointer_traits<pointer>;
    using const_pointer      = typename pointer_traits::rebind<const value_type>;
    using void_pointer       = typename pointer_traits::rebind<void>;
    using const_void_pointer = typename pointer_traits::rebind<const void>;
    using difference_type    = typename pointer_traits::difference_type;

    // Using unsigned integer underflow, we should get the highest value. 
    static constexpr size_type upper_limit = static_cast<size_type>(-1);
    
    static constexpr size_type max_size() noexcept {
        return (upper_limit / sizeof(value_type)) - 1;
    }

    static pointer allocate(allocator_type &allocator, size_type n_count)
    {
        if (n_count == 0) {
            return nullptr;
        } else if (n_count > max_size()) {
            throw std::bad_array_new_length();
        }
        return allocator.allocate(n_count);
    }
    
    static void deallocate(allocator_type &allocator, size_type n_count)
    {
        (void)n_count;
        allocator.deallocate();
    }
    
    /** 
     * Double colons before `new` ensures we resolve from global namespace.
     * With placement new, we can construct objects in allocated memory blocks. 
     *  - https://en.cppreference.com/w/cpp/memory/allocator_traits/construct 
     *  - https://en.cppreference.com/w/cpp/memory/allocator/construct 
     */
    template<class T, class ...Args>
    static void construct(T *p_memory, Args &&...args)
    {
        ::new (static_cast<void*>(p_memory)) T(std::forward<Args>(args)...);
    }

    /** 
     * At the given location `p_memory`, call the destructor of type `T`.
     *  - https://en.cppreference.com/w/cpp/memory/destroy_at 
     */
    template<class T>
    static void destroy(T *p_memory)
    {
        p_memory->~T(); // For fundamentals types this is just a no-op.
    }
};

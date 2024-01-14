/* -*- C STANDARD LIBRARY -*- */
#include <cstdlib> /* std::malloc, std::free */

/* -*- C++ STANDARD LIBRARY -*- */
#include <stdexcept> /* std::out_of_range */
#include <new> /* std::bad_array_new_length, std::bad_alloc */

/* -*- CRIM "LIBRARY" -*- */
#include "logerror.hpp"

#define crim_allocator_logerror(fn, info) \
    crim_logerror("crim::allocator<T>::" fn, info)

namespace crim {
    /**
     * Custom allocator to be used by `std::allocator_traits`. 
     *  - https://learn.microsoft.com/en-us/cpp/standard-library/allocators?view=msvc-170#writing-your-own-allocator-c11k
     */
    template<class ElemT> struct allocator;
};

template<class ElemT> struct crim::allocator {
    static constexpr size_t ALLOCATIONS_MIN = 1;
    static constexpr size_t ALLOCATIONS_MAX = SIZE_MAX / sizeof(ElemT);

    // Default constructor is not required by the C++ standard library.
    allocator() noexcept {}

    // Converting copy-constructor.
    template<class OtherT> allocator(const allocator<OtherT> &other) noexcept {}

    // Prevent users from requesting a size of 0 or something ridiculously big.
    static constexpr bool is_valid_size(size_t n_count) {
        return n_count >= ALLOCATIONS_MIN && n_count < ALLOCATIONS_MAX;
    }

    /**  
     * @brief   Validates given size before trying to allocate a memory block.
     *
     * @param   n_count     How many *elements*, not *bytes* are desired.
     * 
     * @exception   `std::bad_array_new_length()`, `std::bad_alloc()`.
    */
    ElemT *allocate(size_t n_count) const {
        if (!is_valid_size(n_count)) {
            // This may be a bad idea, std::allocator doesn't do this.
            if (n_count == 0) {
                return nullptr; 
            }
            crim_allocator_logerror("allocate", "requested too much memory!");
            throw std::bad_array_new_length();
        }
        // Running out of memory IS an exceptional situation.
        //  - https://stackoverflow.com/a/4827445
        void *p_memory = std::malloc(sizeof(ElemT) * n_count);
        if (p_memory == nullptr) {
            crim_allocator_logerror("allocate", "failed to allocate memory!");
            throw std::bad_alloc();
        }
        return static_cast<ElemT*>(p_memory); 
    }
    
    /** 
     * Double colons before `new` ensures we resolve from global namespace.
     * With placement new, we can construct objects in allocated memory blocks. 
     *  - https://en.cppreference.com/w/cpp/memory/allocator_traits/construct 
     *  - https://en.cppreference.com/w/cpp/memory/allocator/construct 
     */
    template<class CtorT, class ...Args>
    void construct_at(CtorT *p_memory, Args &&...args) const {
        ::new (static_cast<void*>(p_memory)) ElemT(std::forward<Args>(args)...);
    }

    /** 
     * Destructors can be called on *type-names*, but not on fundamental types.
     * Typedefs & template parameters of fundamental types are OK. However, 
     * calling destructors on fundamental types themselves isn't. 
     * ```cpp
     * p_memory->~int(); // is not allowed 
     * p_memory->~ElemT(); // is allowed
     * ```
     *  - https://en.cppreference.com/w/cpp/memory/destroy_at 
     */
    template<class DtorT>
    void destroy_at(DtorT *p_memory) const {
        p_memory->~DtorT(); // For fundamentals types this is just a no-op.
    }
    
    void deallocate(ElemT *p_memory, size_t n_count) const noexcept {
        (void)n_count; // The signature is needed but we don't need n_count.
        std::free(p_memory);
    }
};

#undef crim_allocator_logerror

/* -*- C STANDARD LIBRARY -*- */
#include <stdlib.h>

/* -*- C++ STANDARD LIBRARY -*- */
#include <stdexcept>

namespace crim {
    // Custom allocator to be used by std::allocator traits.
    // https://learn.microsoft.com/en-us/cpp/standard-library/allocators?view=msvc-170#writing-your-own-allocator-c11
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
            return (n_count == 0) ? nullptr : throw std::bad_array_new_length();
        }
        // Running out of memory IS an exceptional situation.
        //  - https://stackoverflow.com/a/4827445
        void *p_memory = malloc(sizeof(ElemT) * n_count);
        if (p_memory == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<ElemT*>(p_memory); 
    }
    
    // Double colons before `new` ensures we resolve from the global namespace.
    // Using placement new, we can construct an object in allocated memory.
    //  - https://en.cppreference.com/w/cpp/memory/allocator_traits/construct
    //  - https://en.cppreference.com/w/cpp/memory/allocator/construct
    template<class CtorT, class ...Args>
    void construct_at(CtorT *p_memory, Args &&...args) const {
        ::new (static_cast<void*>(p_memory)) ElemT(std::forward<Args>(args)...);
    }

    /** 
     * @brief   Destructors can be called on *type-names*, not fundamental types.
     *
     * @details Typedefs & template parameters of fundamental types are OK.
     *          but not destructing fundamental types themselves. 
     *          `p_memory->~int()` is not allowed but `p_memory->~ElemT()` is. 
     *          See: https://en.cppreference.com/w/cpp/memory/destroy_at 
     */
    template<class DtorT>
    void destroy_at(DtorT *p_memory) const {
        p_memory->~DtorT();
    }

    void deallocate(ElemT *p_memory, size_t n_count) const noexcept {
        (void)n_count; // The signature is needed but we don't need n_count.
        free(p_memory);
    }
};

#pragma once

#include "base_dyarray.tcc"

namespace crim {
    template<class ElemT> class dyarray;
};

/**
 * @brief   Programmer-facing version of `crim::base_dyarray`. You're meant 
 *          to use this with fundamental/user-defined types only.
 * 
 * @tparam  ElemT   Desired type of the buffer's elements.
 * 
 * @warning This is a work in progress!
 */
template<typename ElemT> 
class crim::dyarray : public crim::base_dyarray<crim::dyarray<ElemT>, ElemT> {
private:
    using base = base_dyarray<dyarray<ElemT>, ElemT>;
public:
    // Delegate's to base class's constructor for no arguments.
    // Does some basic allocations.
    dyarray() : base() {}

    // Delegates to base class's constructor for initializer lists.
    // Think of them like array literals.
    dyarray(std::initializer_list<ElemT> list) : base(list) {}

    dyarray(const dyarray &src) : base(src) {}

    // Static cast is necessary to call the correct function, otherwise it'll
    // use the one meant for lvalue references!
    dyarray(dyarray &&src) : base(static_cast<base&&>(src)) {}

    dyarray &operator=(const dyarray &src) {
        return base::copy(src);
    }

    dyarray &operator=(dyarray &&src) {
        return base::move(src);
    }
};

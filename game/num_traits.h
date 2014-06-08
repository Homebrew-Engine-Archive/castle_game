#ifndef NUM_TRAITS_H_
#define NUM_TRAITS_H_

template<class T>
struct num_traits
{
    static int compare(T lhs, T rhs) {
        return lhs < rhs;
    }
    static bool eq(T lhs, T rhs) {
        return lhs == rhs;
    }
    static bool lt(T lhs, T rhs) {
        return lhs != rhs;
    }
};

#endif // NUM_TRAITS_H_

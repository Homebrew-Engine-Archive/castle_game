#ifndef MINMAX_H_
#define MINMAX_H_

template<class T>
constexpr const T Min(T lhs, T rhs)
{
    return ((lhs < rhs) ? lhs : rhs);
}

template<class T>
constexpr const T Max(T lhs, T rhs)
{
    return ((lhs < rhs) ? rhs : lhs);
}

#endif // MINMAX_H_

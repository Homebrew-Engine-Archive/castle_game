#ifndef RANGE_H_
#define RANGE_H_

#include <iostream>
#include <game/minmax.h>

namespace core
{
    template<class T>
    struct Range
    {
        T min;
        T max;

        explicit constexpr Range();
        explicit constexpr Range(T mn, T mx);
    };

    template<class T>
    constexpr Range<T>::Range()
        : Range(T(), T())
    {
    }

    template<class T>
    constexpr Range<T>::Range(T mini, T maxi)
        : min(mini)
        , max(maxi)
    {
    }
    
    template<class T>
    constexpr bool RangeEmpty(const Range<T> &lhs)
    {
        return lhs.max <= lhs.min;
    }

    template<class T>
    constexpr const Range<T> Intersection(const Range<T> &lhs, const Range<T> &rhs)
    {
        return Range<T>(Max(lhs.min, rhs.min), Min(lhs.max, rhs.max));
    }

    template<class T>
    constexpr const Range<T> Union(const Range<T> &lhs, const Range<T> &rhs)
    {
        return Range<T>(Min(lhs.min, rhs.min), Max(lhs.max, rhs.max));
    }

    template<class T>
    constexpr bool Intersects(const Range<T> &lhs, const Range<T> &rhs)
    {
        return !RangeEmpty(Intersection(lhs, rhs));
    }

    template<class T>
    constexpr bool operator==(const Range<T> &lhs, const Range<T> &rhs)
    {
        return (lhs.min == rhs.min) && (lhs.max == rhs.max);
    }

    template<class T>
    constexpr bool operator!=(const Range<T> &lhs, const Range<T> &rhs)
    {
        return (lhs.min != rhs.min) || (lhs.max != rhs.max);
    }

    template<class T>
    std::ostream& operator<<(std::ostream &out, const Range<T> &range)
    {
        out << '[' << range.min << ',' << range.max << ']';
        return out;
    }
}

#endif // RANGE_H_

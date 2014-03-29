#ifndef MACROSOTA_H_
#define MACROSOTA_H_

#include <memory>

namespace std
{

#ifndef make_unique
    /** 
     * \brief C++14's std::make_unique forward defintion :D
     * \return Pointer to constructed object with given args.
     */
    template<class T, class ...U>
    std::unique_ptr<T> make_unique(U&&... u)
    {
        return std::unique_ptr<T>(new T(std::forward<U>(u)...));
    }
#endif

    template<class A, class B>
    struct hash<pair<A, B>>
    {
        size_t operator()(pair<A, B> const& p)
        {
            return hash<A>()(p.first) ^ (1 << hash<B>()(p.second));
        }
    };

    template<class A, class B>
    struct equal_to<pair<A, B>>
    {
        bool operator()(pair<A, B> const& lhs, pair<A, B> const& rhs)
        {
            return equal_to<A>()(lhs.first, rhs.first)
                && equal_to<B>()(lhs.second, rhs.second);
        }
    };
    
} // namespace std

#endif

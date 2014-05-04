#ifndef MAKE_UNIQUE_H_
#define MAKE_UNIQUE_H_

#include <memory>

namespace std
{

#ifndef make_unique
    /** 
     * \brief C++1y's std::make_unique forward defintion :D
     * \return Pointer to constructed object with given args.
     */
    template<class T, class ...U>
    std::unique_ptr<T> make_unique(U&&... u)
    {
        return std::unique_ptr<T>(new T(std::forward<U>(u)...));
    }
#endif

} // namespace std

#endif

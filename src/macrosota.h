#ifndef MACROSOTA_H_
#define MACROSOTA_H_

#include <memory>

/** 
 * \brief C++14's std::make_unique forward defintion :D
 * \return Pointer to constructed object with given args.
 */
template<class T, class ...U>
std::unique_ptr<T> make_unique(U&&... u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)...));
}

#define UNUSED(name) (void)name;

#define MAKE_COMPILER_HAPPY(name) UNUSED(name)

#endif

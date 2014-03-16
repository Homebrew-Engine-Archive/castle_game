#ifndef MACROSOTA_H_
#define MACROSOTA_H_

#include <memory>

/**
 * C++14's std::make_unique forward defintion :D
 */
template<class T, class ...U>
std::unique_ptr<T> make_unique(U&&... u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)...));
}

#define NAMESPACE_BEGIN(ns) namespace ns {
#define NAMESPACE_END(ns) }

#define UNUSED(name) (void)name;

#define MAKE_COMPILER_HAPPY(name) UNUSED(name)

#endif

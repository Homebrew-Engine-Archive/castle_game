#ifndef MODULO_H_
#define MODULO_H_

namespace core
{
    template<class T>
    constexpr T Modulo(T a, T b)
    {
        return (((a % b) + b) % b);
    }

    template<class T>
    constexpr bool Even(T a)
    {
        return Modulo(a, static_cast<T>(2)) == static_cast<T>(0);
    }

    template<class T>
    constexpr bool Odd(T a)
    {
        return Modulo(a, static_cast<T>(2)) == static_cast<T>(1);
    }
}

#endif // MODULO_H_

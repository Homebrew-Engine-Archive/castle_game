#ifndef CLAMP_H_
#define CLAMP_H_

namespace core
{
    template<class T>
    constexpr T Clamp(T value, T min, T max)
    {
        return ((value < min)
                ? min
                : ((value > max)
                   ? max
                   : value));
    }
}

#endif // CLAMP_H_

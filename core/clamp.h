#ifndef CLAMP_H_
#define CLAMP_H_

namespace core
{
    template<typename T, typename U>
    constexpr T Clamp(T value, U min, U max)
    {
        return ((value < min)
                ? min
                : ((value > max)
                   ? max
                   : value));
    }
}

#endif // CLAMP_H_

#ifndef VMATH_H_
#define VMATH_H_

#include <iostream>

namespace core
{
    template<class T>
    struct Vec2
    {
        union {
            struct {
                T x;
                T y;
            } xy;
            struct {
                T w;
                T h;
            } wh;
            struct {
                T a;
                T b;
            } ab;
        };
    
        constexpr Vec2(T firstcomp, T secondcomp)
            : xy {firstcomp, secondcomp}
            {}

        constexpr Vec2()
            : Vec2(0, 0)
            {}

        inline Vec2<T>& operator+=(const Vec2<T> &x) {
            xy.x += x.xy.x;
            xy.y += x.xy.y;
            return *this;
        }

        inline Vec2<T>& operator-=(const Vec2<T> &x) {
            xy.x -= x.xy.x;
            xy.y -= x.xy.y;
            return *this;
        }
    };

    typedef Vec2<uint32_t> Vec2i;
    typedef Vec2<float> Vec2f;

    template<class T>
    const Vec2<T> operator+(const Vec2<T> &a, const Vec2<T> &b)
    {
        Vec2<T> tmp(a);
        tmp += b;
        return tmp;
    }

    template<class T>
    const Vec2<T> operator-(const Vec2<T> &a, const Vec2<T> &b)
    {
        Vec2<T> tmp(a);
        tmp += b;
        return tmp;
    }
    
    template<class T>
    std::ostream& operator<<(std::ostream &out, const Vec2<T> &vec2)
    {
        out << '(' << vec2.xy.x << ',' << vec2.xy.y << ')';
        return out;
    }
}

#endif // VMATH_H_

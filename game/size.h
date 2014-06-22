#ifndef SIZE_H_
#define SIZE_H_

#include <SDL.h>

namespace core
{
    template<class T>
    struct SizeT
    {
        T width;
        T height;

        constexpr inline explicit SizeT(T w = T(), T h = T())
            : width(w)
            , height(h)
            {}
    };

    /** "Specialize" SizeT template for int and float using different names **/
    struct Size : public SizeT<int>
    {
        /** Inheriting constructors require gcc-4.8 and newer **/
        using SizeT::SizeT;
    };

    struct SizeF : public SizeT<float>
    {
        using SizeT::SizeT;
    };

    constexpr Size RectSize(const SDL_Rect &rect)
    {
        return Size(rect.w, rect.h);
    }

    constexpr Size FromPoint(const SDL_Point &point)
    {
        return Size(point.x, point.y);
    }
}

#endif // SIZE_H_

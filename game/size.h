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

        constexpr SizeT(T w, T h)
            : width(w)
            , height(h)
            {}

        constexpr SizeT()
            : SizeT(T(), T())
            {}
    };

    struct Size : public SizeT<int>
    {
        /** Inheriting constructors require gcc-4.8 and newer **/
        using SizeT::SizeT;
    };

    struct SizeF : public SizeT<double>
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

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
            : width(std::move(w))
            , height(std::move(h))
            {}

        constexpr SizeT()
            : SizeT(0, 0)
            {}
    };

    using Size = SizeT<uint32_t>;
    using Sizef = SizeT<float>;

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

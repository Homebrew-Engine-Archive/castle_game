#ifndef BITMAP_H_
#define BITMAP_H_

#include <gmtool/renderer.h>

namespace castle
{
    class Image;
}

namespace gmtool
{
    struct BitmapFormat : public renderer
    {
        void rendero_SDL_RWops(SDL_RWops *dst, const castle::Image &surface);
    };
}

#endif // BITMAP_H_


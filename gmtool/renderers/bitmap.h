#ifndef BITMAP_H_
#define BITMAP_H_

#include <gmtool/renderer.h>

namespace core
{
    class Image;
}

namespace gmtool
{
    struct BitmapFormat : public Renderer
    {
        void RenderToSDL_RWops(SDL_RWops *dst, const core::Image &surface);
    };
}

#endif // BITMAP_H_


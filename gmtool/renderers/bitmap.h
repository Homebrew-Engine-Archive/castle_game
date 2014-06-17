#ifndef BITMAP_H_
#define BITMAP_H_

#include <gmtool/renderer.h>

namespace Castle
{
    class Image;
}

namespace GMTool
{
    struct BitmapFormat : public Renderer
    {
        void RenderTo_SDL_RWops(SDL_RWops *dst, const Castle::Image &surface);
    };
}

#endif // BITMAP_H_


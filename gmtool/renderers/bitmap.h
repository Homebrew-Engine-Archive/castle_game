#ifndef BITMAP_H_
#define BITMAP_H_

#include <gmtool/renderer.h>

class Surface;

namespace GMTool
{
    struct BitmapFormat : public Renderer
    {
        void RenderTo_SDL_RWops(SDL_RWops *dst, const Surface &surface);
    };
}

#endif // BITMAP_H_


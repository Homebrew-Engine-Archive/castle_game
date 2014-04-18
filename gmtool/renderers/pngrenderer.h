#ifndef PNGRENDERER_H_
#define PNGRENDERER_H_

#include <gmtool/renderer.h>

namespace GMTool
{
    struct PNGRenderer : public Renderer
    {
        void RenderTo_SDL_RWops(SDL_RWops *dst, const Surface &surface);
    };
}

#endif // PNGRENDERER_H_

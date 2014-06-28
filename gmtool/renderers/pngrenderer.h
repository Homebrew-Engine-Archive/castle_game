#ifndef PNGRENDERER_H_
#define PNGRENDERER_H_

#include <gmtool/renderer.h>

namespace gmtool
{
    struct PNGRenderer : public Renderer
    {
        void RenderToSDL_RWops(SDL_RWops *dst, const core::Image &surface);
    };
}

#endif // PNGRENDERER_H_

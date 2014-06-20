#ifndef PNGRENDERER_H_
#define PNGRENDERER_H_

#include <gmtool/renderer.h>

namespace gmtool
{
    struct PNGrenderer : public renderer
    {
        void rendero_SDL_RWops(SDL_RWops *dst, const castle::Image &surface);
    };
}

#endif // PNGRENDERER_H_

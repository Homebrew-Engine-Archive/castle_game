#include "pngrenderer.h"

#include <stdexcept>

#include <game/image.h>

#include <SDL_image.h>

namespace gmtool
{
    void PNGRenderer::RenderToSDL_RWops(SDL_RWops *out, const castle::Image &image)
    {
        IMG_Init(IMG_INIT_PNG);
        if(IMG_SavePNG_RW(image.GetSurface(), out, SDL_FALSE) < 0) {
            throw std::runtime_error(IMG_GetError());
        }
        IMG_Quit();
    }
}

#include "pngrenderer.h"

#include <game/surface.h>

#include <stdexcept>
#include <SDL_image.h>

namespace GMTool
{
    void PNGRenderer::RenderTo_SDL_RWops(SDL_RWops *dst, const Surface &surface)
    {
        IMG_Init(IMG_INIT_PNG);
        
        if(IMG_SavePNG_RW(surface, dst, SDL_FALSE) < 0) {
            throw std::runtime_error(IMG_GetError());
        }
        IMG_Quit();
    }
}

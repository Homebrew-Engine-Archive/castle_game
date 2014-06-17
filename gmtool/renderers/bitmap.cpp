#include "bitmap.h"

#include <SDL.h>

#include <game/image.h>

namespace GMTool
{
    void BitmapFormat::RenderTo_SDL_RWops(SDL_RWops *dst, const Castle::Image &surface)
    {
        SDL_SaveBMP_RW(surface, dst, 0);
    }
}

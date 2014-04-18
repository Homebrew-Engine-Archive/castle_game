#include "bitmap.h"

#include <game/surface.h>
#include <SDL.h>

namespace GMTool
{
    void BitmapFormat::RenderTo_SDL_RWops(SDL_RWops *dst, const Surface &surface)
    {
        SDL_SaveBMP_RW(surface, dst, 0);
    }
}

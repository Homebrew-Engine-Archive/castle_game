#include "bitmap.h"

#include <SDL.h>

#include <game/image.h>

namespace gmtool
{
    void BitmapFormat::RenderToSDL_RWops(SDL_RWops *out, const castle::Image &image)
    {
        SDL_SaveBMP_RW(image.GetSurface(), out, SDL_FALSE);
    }
}

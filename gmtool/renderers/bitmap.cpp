#include "bitmap.h"

#include <SDL.h>

#include <core/image.h>

namespace gmtool
{
    void BitmapFormat::RenderToSDL_RWops(SDL_RWops *out, const core::Image &image)
    {
        SDL_SaveBMP_RW(image.GetSurface(), out, SDL_FALSE);
    }
}

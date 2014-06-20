#include "bitmap.h"

#include <SDL.h>

#include <game/image.h>

namespace gmtool
{
    void BitmapFormat::rendero_SDL_RWops(SDL_RWops *dst, const castle::Image &surface)
    {
        SDL_SaveBMP_RW(surface, dst, 0);
    }
}

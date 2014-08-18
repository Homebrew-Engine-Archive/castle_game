#include "bitmapwriter.h"
#include <SDL.h>
#include <core/image.h>

namespace gmtool
{
    void BitmapWriter::Write(SDL_RWops *out, const core::Image &image) const
    {
        SDL_SaveBMP_RW(image.GetSurface(), out, SDL_FALSE);
    }
}

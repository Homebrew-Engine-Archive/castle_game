#include "bitmapwriter.h"
#include <SDL.h>
#include <core/image.h>
#include <core/sdl_error.h>

namespace gmtool
{
    void BitmapWriter::Write(SDL_RWops *out, const core::Image &image) const
    {
        if(SDL_SaveBMP_RW(image.GetSurface(), out, SDL_FALSE) < 0) {
            throw sdl_error();
        }
    }
}

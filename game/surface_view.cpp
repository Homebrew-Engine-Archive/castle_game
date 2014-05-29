#include "surface_view.h"

#include <SDL.h>

#include <stdexcept>

#include <game/sdl_error.h>
#include <game/rect.h>

const Surface CreateSurfaceView(Surface &src, const Rect &clip)
{
    if(SDL_MUSTLOCK(src)) {
        // \todo can we deal with it?
        throw std::invalid_argument("SurfaceView might not be created from RLEaccel surface");
    }

    const Rect cropped =
        IntersectRects(
            Normalized(clip),
            Rect(src));

    char *const pixels = GetPixels(src)
        + cropped.y * src->pitch
        + cropped.x * src->format->BytesPerPixel;

    Surface tmp;
    tmp = CreateSurfaceFrom(pixels, cropped.w, cropped.h, src->pitch, *src->format);
    if(!tmp) {
        throw sdl_error();
    }
    CopyColorKey(src, tmp);
    
    return tmp;
}

SurfaceView::SurfaceView(Surface &src, const Rect &clip)
    : mSurface(CreateSurfaceView(src, clip))
    , mParentRef(src)
{
}

// \todo there is something wrong with const-specifier
// problem arises to SDL_CreateRGBSurfaceFrom which asks for non-const pixels
// We can just copy object to remove const-cv anyway.
SurfaceView::SurfaceView(const Surface &src, const Rect &clip)
    : SurfaceView(const_cast<Surface&>(src), clip)
{
}

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
        Intersection(
            Normalized(clip),
            Rect(src));

    char *const data = SurfaceData(src)
        + cropped.y * SurfaceRowStride(src)
        + cropped.x * SurfacePixelStride(src);

    Surface tmp;
    tmp = CreateSurfaceFrom(data, cropped.w, cropped.h, SurfaceRowStride(src), SurfaceFormat(src));
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

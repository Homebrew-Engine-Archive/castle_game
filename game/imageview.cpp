#include "imageview.h"

#include <SDL.h>

#include <stdexcept>

#include <game/color.h>
#include <game/sdl_error.h>
#include <game/rect.h>

namespace Castle
{
    const Image CreateImageView(Image &src, const core::Rect &clip)
    {
        if(SDL_MUSTLOCK(src)) {
            // \todo can we deal with it?
            throw std::invalid_argument("ImageView might not be created from RLEaccel surface");
        }

        ImageLocker lock(src);
        
        const core::Rect cropped =
            Intersection(
                core::Normalized(clip),
                core::Rect(src));

        char *const data = lock.Data()
            + cropped.y * src.RowStride()
            + cropped.x * src.PixelStride();

        Image tmp;
        tmp = CreateImageFrom(data, cropped.w, cropped.h, src.RowStride(), ImageFormat(src));
        if(src.HasColorKey()) {
            tmp.SetColorKey(src.GetColorKey());
        }
    
        return tmp;
    }

    ImageView::ImageView(Image &src, const core::Rect &clip)
        : mImage(CreateImageView(src, clip))
        , mParentRef(src)
    {
    }

// \todo there is something wrong with const-specifier
// problem arises to SDL_CreateRGBSurfaceFrom which asks for non-const pixels
// We can just copy object to remove const-cv anyway.
    ImageView::ImageView(const Image &src, const core::Rect &clip)
        : ImageView(const_cast<Image&>(src), clip)
    {
    }
}

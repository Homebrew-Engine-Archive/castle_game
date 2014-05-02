#include "texturelocker.h"

#include <boost/current_function.hpp>
#include <game/exception.h>

namespace Render
{
    TextureLocker::TextureLocker(SDL_Texture *texture)
        : mTexture(texture)
        , mPixels(nullptr)
        , mPitch(0)
    {
        if(texture != NULL) {
            if(SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) < 0) {
                throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
            }
        }
    }

    TextureLocker::~TextureLocker()
    {
        if(mTexture != NULL) {
            SDL_UnlockTexture(mTexture);
        }
    }

    void* TextureLocker::Pixels() const
    {
        return mPixels;
    }

    int TextureLocker::Pitch() const
    {
        return mPitch;
    }
}

#include "texturelocker.h"

#include <game/sdl_error.h>

namespace Render
{
    TextureLocker::TextureLocker(SDL_Texture *texture)
        : mTexture(texture)
        , mPixels(nullptr)
        , mPitch(0)
    {
        if(texture != NULL) {
            if(SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) < 0) {
                throw sdl_error();
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

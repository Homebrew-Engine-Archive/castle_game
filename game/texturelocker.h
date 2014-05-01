#ifndef TEXTURELOCKER_H_
#define TEXTURELOCKER_H_

#include <SDL.h>

namespace Render
{
    class TextureLocker
    {
        SDL_Texture *mTexture;
        void *mPixels;
        int mPitch;
    public:
        TextureLocker(SDL_Texture *texture);
        TextureLocker(TextureLocker const&) = delete;
        TextureLocker& operator=(TextureLocker const&) = delete;
        ~TextureLocker();

        void* Pixels() const;
        int Pitch() const;
    };
}

#endif // TEXTURELOCKER_H_

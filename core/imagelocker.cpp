#include "imagelocker.h"

#include <SDL.h>
#include <exception>

#include <core/image.h>

namespace core
{
    ImageLocker::ImageLocker(const Image &surface)
        : mImage(surface)
        , mLocked(false)
    {
        if(!mImage.Null()) {
            if(SDL_MUSTLOCK(mImage.GetSurface())) {
                if(0 == SDL_LockSurface(mImage.GetSurface())) {
                    mLocked = true;
                }
            }
        }
    }

    void ImageLocker::Unlock()
    {
        if(mLocked && !mImage.Null()) {
            SDL_UnlockSurface(mImage.GetSurface());
        }
    }
    
    ImageLocker::~ImageLocker() throw()
    {
        try {
            if(!mImage.Null()) {
                Unlock();
            }
        } catch(const std::exception &error) {
            std::cerr << "unable to unlock surface: " << error.what() << std::endl;
        }
    }

    
    char const* ImageLocker::Data() const
    {
        return reinterpret_cast<const char*>(mImage.GetSurface()->pixels);
    }
    
    char* ImageLocker::Data()
    {
        return reinterpret_cast<char*>(mImage.GetSurface()->pixels);
    }
}

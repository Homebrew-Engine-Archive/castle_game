#ifndef IMAGELOCKER_H_
#define IMAGELOCKER_H_

namespace core
{
    class Image;
}

namespace core
{
    /**
       \brief RAII for SDL_LockSurface and SDL_UnlockSurface functions call
    **/
    class ImageLocker
    {
        const core::Image &mImage;
        bool mLocked;
        
    public:
        explicit ImageLocker(const core::Image &surface);
        ImageLocker(ImageLocker const&) = delete;
        ImageLocker &operator=(ImageLocker const&) = delete;
        virtual ~ImageLocker() throw();

        void Unlock();
        
        char const* Data() const;
        char* Data();
    };
}

#endif // IMAGELOCKER_H_

#ifndef IMAGELOCKER_H_
#define IMAGELOCKER_H_

namespace castle
{
    class Image;
}

namespace castle
{
    /**
       \brief RAII for SDL_LockSurface and SDL_UnlockSurface functions call
    **/
    class ImageLocker
    {
        const castle::Image &mImage;
        bool mLocked;
        
    public:
        explicit ImageLocker(const castle::Image &surface);
        ImageLocker(ImageLocker const&) = delete;
        ImageLocker &operator=(ImageLocker const&) = delete;
        virtual ~ImageLocker() throw();

        void Unlock();
        
        char const* Data() const;
        char* Data();
    };
}

#endif // IMAGELOCKER_H_

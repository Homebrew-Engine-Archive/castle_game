#ifndef IMAGE_H_
#define IMAGE_H_

#include <cassert>
#include <SDL.h>
#include <game/rect.h>

namespace core
{
    class Point;
    class Color;
}

namespace castle
{
    class Palette;
}

namespace castle
{
    /**
       Software memory image block.
     **/
    class Image
    {
    protected:
        SDL_Surface *mSurface;
        void Assign(SDL_Surface*);
    
    public:
        Image();
        explicit Image(SDL_Surface*);
        Image(Image const&);
        virtual ~Image();
        
        bool Null() const;
        operator SDL_Surface*() const;
        bool operator!() const;
        Image& operator=(SDL_Surface*);
        Image& operator=(Image const&);
        SDL_Surface* operator->() const;

        inline size_t Width() const;
        inline size_t Height() const;
        size_t RowStride() const;
        size_t PixelStride() const;

        bool HasColorKey() const;
        void DisableColorKey();
        void SetColorKey(const core::Color &color);
        const core::Color GetColorKey() const;

        void SetClipRect(const core::Rect &clipRect);
        const core::Rect GetClipRect() const;

        using BlendMode = SDL_BlendMode;
        BlendMode GetBlendMode() const;
        void SetBlendMode(BlendMode mode);

        void AttachPalette(Palette &palette);
    };

    inline size_t Image::Width() const
    {
        assert(!Null());
        return mSurface->w;
    }

    inline size_t Image::Height() const
    {
        assert(!Null());
        return mSurface->h;
    }
    
/**
   \brief RAII for SDL_LockSurface and SDL_UnlockSurface functions call
**/
    class ImageLocker
    {
        Image mImage;
        bool mLocked;
        
    public:
        explicit ImageLocker(const Image &surface);
        ImageLocker(ImageLocker const&) = delete;
        ImageLocker &operator=(ImageLocker const&) = delete;
        virtual ~ImageLocker() throw();

        void Unlock();
        
        char const* Data() const;
        char* Data();
    };

    class ImageAlphaModSetter
    {
        Image mImage;
        uint8_t alphaMod;
        
    public:
        explicit ImageAlphaModSetter(const Image &src, uint8_t alpha);
        virtual ~ImageAlphaModSetter() throw();

        void Rollback();
    };

    class ImageClipper
    {
        Image mImage;
        core::Rect mOldClip;
    
    public:
        explicit ImageClipper(Image &surface, const core::Rect &cliprect);
        virtual ~ImageClipper() throw();

        void Rollback();
    };

    bool IsPalettized(const Image &surface);
    bool IsIndexed(const SDL_PixelFormat &format);
    bool IsRGB(const SDL_PixelFormat &format);
    bool IsARGB(const SDL_PixelFormat &format);

    SDL_PixelFormat const& ImageFormat(const Image &surface);

    const Image CreateImage(int width, int height, const SDL_PixelFormat &format);
    const Image CreateImage(int width, int height, uint32_t format);

    const Image ConvertImage(const Image &source, uint32_t format);
    const Image ConvertImage(const Image &source, const SDL_PixelFormat &format);

    const Image CreateImageFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat &format);
    const Image CreateImageFrom(void *pixels, int width, int height, int pitch, uint32_t format);

    void CopyImage(const Image &source, const core::Rect &sourceRect, Image &target, const core::Point &targetPoint);
    void CopyImageScaled(const Image &source, const core::Rect &sourceRect, Image &target, const core::Rect &targetRect);

    uint32_t ExtractPixel(const Image &surface, const core::Point &coord);
}

#endif

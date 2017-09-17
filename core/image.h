#ifndef IMAGE_H_
#define IMAGE_H_

#include <cassert>

#include <SDL.h>

#include <core/rect.h>
#include <core/color.h>

namespace core
{
    class Point;
}

namespace core
{
    class Palette;
}

namespace core
{
    /**
       Software memory image block.
     **/
    class Image
    {
    private:
        mutable SDL_Surface *mSurface;
        core::Color mColorKey;
        bool mColorKeyEnabled;
    
    public:
        Image();
        Image(Image const&);
        explicit Image(SDL_Surface*);

        Image& operator=(Image const&);
        Image& operator=(SDL_Surface*);
        virtual ~Image();
        
        bool Null() const;
        bool operator!() const;

        inline size_t Width() const;
        inline size_t Height() const;
        size_t RowStride() const;
        size_t PixelStride() const;

        bool ColorKeyEnabled() const;
        void EnableColorKey(bool enabled);
        void SetColorKey(const core::Color &color);
        const core::Color GetColorKey() const;

        void SetClipRect(const core::Rect &clipRect);
        const core::Rect GetClipRect() const;

        using BlendMode = SDL_BlendMode;
        BlendMode GetBlendMode() const;
        void SetBlendMode(BlendMode mode);

        void SetOpacity(int opacity);
        int GetOpacity() const;

        void AttachPalette(Palette &palette);

        inline SDL_Surface* GetSurface() const;
    };

    inline SDL_Surface* Image::GetSurface() const
    {
        return mSurface;
    }
    
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

    void ClearImage(Image &source, const core::Color &clearColor);
    
    void CopyImage(const Image &source, const core::Rect &sourceRect, Image &target, const core::Point &targetPoint);
    void CopyImageScaled(const Image &source, const core::Rect &sourceRect, Image &target, const core::Rect &targetRect);

    uint32_t ExtractPixel(const Image &surface, const core::Point &coord);
}

#endif

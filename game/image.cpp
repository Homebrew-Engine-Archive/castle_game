#include "image.h"

#include <cassert>

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include <SDL.h>

#include <game/imagedebug.h>
#include <game/imagelocker.h>
#include <game/palette.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/point.h>
#include <game/sdl_utils.h>

namespace
{        
    struct null_surface_error : public std::invalid_argument
    {
        null_surface_error() throw() : std::invalid_argument("surface is null or invalid") {}
    };

    struct null_pixeldata_error : public std::invalid_argument
    {
        null_pixeldata_error() throw() : std::invalid_argument("pixel pointer is null") {}
    };
        
    void AddSurfaceRef(SDL_Surface *surface)
    {
        if(surface != nullptr) {
            ++surface->refcount;
        }
    }
}

namespace castle
{
    Image::Image() : Image(nullptr) {}
        
    Image::Image(SDL_Surface *s)
        : mSurface(s)
        , mColorKey()
        , mColorKeyEnabled(false)
    {
        if(s != nullptr) {
            /** Ensure we are the only instance **/
            assert(s->refcount == 1);
        }
    }
    
    Image::Image(const Image &that)
        : mSurface(that.mSurface)
        , mColorKey(that.mColorKey)
        , mColorKeyEnabled(that.mColorKeyEnabled)
    {
        AddSurfaceRef(mSurface);
    }

    Image& Image::operator=(const Image &that)
    {
        AddSurfaceRef(that.mSurface);

        SDL_Surface *old = mSurface;
        mSurface = that.mSurface;
        mColorKeyEnabled = that.mColorKeyEnabled;
        mColorKey = that.mColorKey;
        SDL_FreeSurface(old);
        return *this;
    }
    
    Image& Image::operator=(SDL_Surface *target)
    {
        if(target != nullptr) {
            assert(target != mSurface);
            assert(target->refcount == 1);
        }
        
        SDL_Surface *old = mSurface;
        mSurface = target;
        SDL_FreeSurface(old);
        return *this;
    }
    
    Image::~Image()
    {
        SDL_FreeSurface(mSurface);
    }

    bool Image::Null() const
    {
        return (mSurface == nullptr);
    }

    bool Image::operator!() const
    {
        return Null();
    }

    bool Image::ColorKeyEnabled() const
    {
        return mColorKeyEnabled;
    }

    void Image::SetColorKey(const core::Color &color)
    {
        mColorKey = color;
        mColorKeyEnabled = true;
    }
    
    void Image::EnableColorKey(bool enabled)
    {
        mColorKeyEnabled = enabled;
    }
    
    const core::Color Image::GetColorKey() const
    {
        return mColorKey;
    }

    size_t Image::RowStride() const
    {
        assert(!Null());
        return mSurface->pitch;
    }

    size_t Image::PixelStride() const
    {
        assert(!Null());
        assert(mSurface->format != NULL);
        return mSurface->format->BytesPerPixel;
    }

    void Image::SetClipRect(const core::Rect &clipRect)
    {
        assert(!Null());
        SDL_SetClipRect(mSurface, &clipRect);
    }
    
    const core::Rect Image::GetClipRect() const
    {
        assert(!Null());
        return core::Rect(mSurface->clip_rect);
    }

    Image::BlendMode Image::GetBlendMode() const
    {
        Image::BlendMode blendMode = SDL_BLENDMODE_NONE;
        if(SDL_GetSurfaceBlendMode(mSurface, &blendMode) < 0) {
            throw sdl_error();
        }
        return blendMode;
    }
    
    void Image::SetBlendMode(Image::BlendMode mode)
    {
        if(SDL_SetSurfaceBlendMode(mSurface, mode) < 0) {
            throw sdl_error();
        }
    }

    void Image::SetOpacity(int opacity)
    {
        if(SDL_SetSurfaceAlphaMod(mSurface, opacity) < 0) {
            throw sdl_error();
        }
    }

    int Image::GetOpacity() const
    {
        uint8_t opacity = 255;
        if(SDL_GetSurfaceAlphaMod(mSurface, &opacity) < 0) {
            throw sdl_error();
        }
        return opacity;
    }    
    
    void Image::AttachPalette(Palette &palette)
    {
        assert(!Null());
        if(SDL_SetSurfacePalette(mSurface, &palette.GetSDLPalette()) < 0) {
            throw sdl_error();
        }
    }
        
    const Image CreateImage(int width, int height, const SDL_PixelFormat &format)
    {
        const uint32_t rmask = format.Rmask;
        const uint32_t gmask = format.Gmask;
        const uint32_t bmask = format.Bmask;
        const uint32_t amask = format.Amask;
        const int bpp = format.BitsPerPixel;
        Image tmp;
        tmp = SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);
        if(!tmp) {
            throw sdl_error();
        }
        return tmp;
    }

    const Image CreateImage(int width, int height, uint32_t format)
    {
        PixelFormatPtr pf(SDL_AllocFormat(format));
        if(!pf) {
            throw sdl_error();
        }
        return CreateImage(width, height, *pf);
    }

    const Image CreateImageFrom(void *pixels, int width, int height, int rowStride, const SDL_PixelFormat &format)
    {
        const uint32_t rmask = format.Rmask;
        const uint32_t gmask = format.Gmask;
        const uint32_t bmask = format.Bmask;
        const uint32_t amask = format.Amask;
        const int bpp = format.BitsPerPixel;
    
        if(pixels != nullptr) {
            Image tmp;
            tmp = SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, rowStride, rmask, gmask, bmask, amask);
            if(!tmp) {
                throw sdl_error();
            }
            return tmp;
        }
        throw null_pixeldata_error();
    }

    const Image CreateImageFrom(void *pixels, int width, int height, int rowStride, uint32_t format)
    {
        PixelFormatPtr pf(SDL_AllocFormat(format));
        if(!pf) {
            throw sdl_error();
        }
        return CreateImageFrom(pixels, width, height, rowStride, *pf);
    }

    const Image ConvertImage(const Image &source, uint32_t format)
    {
        PixelFormatPtr pf(SDL_AllocFormat(format));
        if(!pf) {
            throw sdl_error();
        }
        return ConvertImage(source, *pf);
    }

    const Image ConvertImage(const Image &source, const SDL_PixelFormat &format)
    {
        Image tmp;
        tmp = SDL_ConvertSurface(source.GetSurface(), &format, 0);
    
        if(!tmp) {
            throw sdl_error();
        }
    
        return tmp;
    }

    void UpdateColorKey(const Image &image)
    {
        SDL_Surface *surface = image.GetSurface();
        if(image.ColorKeyEnabled()) {
            const uint32_t colorKey = image.GetColorKey().ConvertTo(ImageFormat(image));
            if(SDL_SetColorKey(surface, SDL_TRUE, colorKey) < 0) {
                throw sdl_error();
            }
        } else {
            if(SDL_SetColorKey(surface, SDL_FALSE, 0) < 0) {
                throw sdl_error();
            }
        }
    }
    
    void CopyImage(const Image &source, const core::Rect &sourceRect, Image &target, const core::Point &targetPoint)
    {
        // tempRect would be modified by SDL_BlitSurface
        core::Rect tempRect(targetPoint, 0, 0);
        UpdateColorKey(source);
        
        if(SDL_BlitSurface(source.GetSurface(), &sourceRect, target.GetSurface(), &tempRect) < 0) {
            throw sdl_error();
        }
    }

    void BlitImageScaled(const Image &source, const core::Rect &sourceRect, Image &dest, const core::Rect &targetRect)
    {
        // see BlitImage
        core::Rect tempRect(targetRect);
        UpdateColorKey(source);
        
        if(SDL_BlitScaled(source.GetSurface(), &sourceRect, dest.GetSurface(), &tempRect) < 0) {
            throw sdl_error();
        }
    }

    uint32_t ExtractPixel(const Image &img, const core::Point &point)
    {
        if(!img) {
            throw null_surface_error();
        }

        if(!core::PointInRect(core::Rect(img.Width(), img.Height()), point)) {
            throw std::invalid_argument("coord out of surface bounds");
        }
    
        const ImageLocker lock(img);
        return core::GetPackedPixel(
            lock.Data()
            + point.y * img.RowStride()
            + point.x * img.PixelStride(), img.PixelStride());
    }

    bool IsPalettized(const Image &image)
    {
        if(!image) {
            throw null_surface_error();
        }
        return IsIndexed(ImageFormat(image));
    }

    bool IsIndexed(const SDL_PixelFormat &format)
    {
        return SDL_ISPIXELFORMAT_INDEXED(format.format);
    }

    bool IsRGB(const SDL_PixelFormat &format)
    {
        return !SDL_ISPIXELFORMAT_ALPHA(format.format);
    }

    bool IsARGB(const SDL_PixelFormat &format)
    {
        return SDL_ISPIXELFORMAT_ALPHA(format.format);
    }

    SDL_PixelFormat const& ImageFormat(const Image &image)
    {
        assert(!image.Null());

        SDL_Surface *surface = image.GetSurface();
        assert(surface->format != nullptr);
        return *surface->format;
    }

    void ClearImage(Image &source, const core::Color &clearColor)
    {
        SDL_Surface *surface = source.GetSurface();
        const uint32_t clearPixel = clearColor.ConvertTo(ImageFormat(source));
        if(SDL_FillRect(surface, NULL, clearPixel) < 0) {
            throw sdl_error();
        }
    }
}

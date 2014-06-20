#include "image.h"

#include <cassert>

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include <SDL.h>

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
    ImageAlphaModSetter::ImageAlphaModSetter(const Image &src, uint8_t newAlphaMod)
        : mImage(src)
    {
        if(SDL_GetSurfaceAlphaMod(mImage, &alphaMod) < 0) {
            throw sdl_error();
        }
        if(SDL_SetSurfaceAlphaMod(mImage, newAlphaMod) < 0) {
            throw sdl_error();
        }
    }

    void ImageAlphaModSetter::Rollback()
    {
        if(SDL_SetSurfaceAlphaMod(mImage, alphaMod) < 0) {
            throw sdl_error();
        }
        mImage = nullptr;
    }

    ImageAlphaModSetter::~ImageAlphaModSetter() throw()
    {
        try {
            if(!mImage.Null()) {
                Rollback();
            }
        } catch(const std::exception &error) {
            std::cerr << "unable recover alpha modifier: " << error.what() << std::endl;
        }
    }

    ImageClipper::ImageClipper(Image &surface, const core::Rect &cliprect)
        : mImage(surface)
        , mOldClip()
    {
        if(!mImage.Null()) {
            mOldClip = surface.GetClipRect();
            surface.SetClipRect(cliprect);
        }
    }

    ImageClipper::~ImageClipper() throw()
    {
        try {
            if(!mImage.Null()) {
                Rollback();
            }
        } catch(const std::exception &error) {
            std::cerr << "unable to recover clip rect: " << error.what() << std::endl;
        }
    }

    void ImageClipper::Rollback()
    {
        if(!mImage.Null()) {
            mImage.SetClipRect(mOldClip);
            mImage = nullptr;
        }
    }
    
    ImageLocker::ImageLocker(const Image &surface)
        : mImage(surface)
        , mLocked(false)
    {
        if(!mImage.Null()) {
            if(SDL_MUSTLOCK(mImage)) {
                if(0 == SDL_LockSurface(mImage)) {
                    mLocked = true;
                }
            }
        }
    }

    void ImageLocker::Unlock()
    {
        if(mLocked && !mImage.Null()) {
            SDL_UnlockSurface(mImage);
            mImage = nullptr;
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

    Image::~Image()
    {
        SDL_FreeSurface(mSurface);
    }

    Image::Image()
        : mSurface(nullptr)
    {
    }

    Image::Image(SDL_Surface *s)
        : mSurface(s)
    {
    }

    Image::Image(const Image &that)
        : mSurface(that.mSurface)
    {
        AddSurfaceRef(mSurface);
    }

    Image& Image::operator=(SDL_Surface *s)
    {
        Assign(s);
        return *this;
    }

    Image& Image::operator=(const Image &that)
    {
        AddSurfaceRef(that.mSurface);
        Assign(that.mSurface);
        return *this;
    }

    void Image::Assign(SDL_Surface *s)
    {
        SDL_Surface *tmp = mSurface;
        mSurface = s;

        /** SDL_FreeSurface decreases reference counter **/
        SDL_FreeSurface(tmp);
    }

    bool Image::Null() const
    {
        return (mSurface == nullptr);
    }

    SDL_Surface* Image::operator->() const
    {
        return mSurface;
    }

    Image::operator SDL_Surface*() const
    {
        return mSurface;
    }

    bool Image::operator!() const
    {
        return Null();
    }

    bool Image::HasColorKey() const
    {
        assert(!Null());
        uint32_t key;
        return (SDL_GetColorKey(mSurface, &key) == 0);
    }

    void Image::SetColorKey(const core::Color &color)
    {
        assert(!Null());
        const uint32_t key = SDL_MapRGBA(mSurface->format, color.r, color.b, color.g, color.a);
        if(SDL_SetColorKey(mSurface, SDL_TRUE, key)) {
            throw sdl_error();
        }
    }
    
    void Image::DisableColorKey()
    {
        assert(!Null());
        SDL_SetColorKey(mSurface, SDL_FALSE, 0);
    }
    
    const core::Color Image::GetColorKey() const
    {
        assert(!Null());
        uint32_t key;
        if(SDL_GetColorKey(mSurface, &key) < 0) {
            throw std::runtime_error("no color key for you, sorry");
        }

        core::Color color;
        SDL_GetRGBA(key, mSurface->format, &color.r, &color.g, &color.b, &color.a);
        return color;
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
        uint32_t rmask;
        uint32_t gmask;
        uint32_t bmask;
        uint32_t amask;
        int bpp;

        if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
            throw sdl_error();
        }
    
        Image tmp;
        tmp = SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);
        if(!tmp) {
            throw sdl_error();
        }
        return tmp;
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
        if(pixels != nullptr) {
            uint32_t rmask;
            uint32_t gmask;
            uint32_t bmask;
            uint32_t amask;
            int bpp;

            if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
                throw sdl_error();
            }
        
            Image tmp;
            tmp = SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, rowStride, rmask, gmask, bmask, amask);
            if(!tmp) {
                throw sdl_error();
            }
            return tmp;
        }
        throw null_pixeldata_error();
    }

    const Image ConvertImage(const Image &source, uint32_t format)
    {
        Image tmp;
        tmp = SDL_ConvertSurfaceFormat(source, format, 0);
    
        if(!tmp) {
            throw sdl_error();
        }
    
        return tmp;
    }

    const Image ConvertImage(const Image &source, const SDL_PixelFormat &format)
    {
        Image tmp;
        tmp = SDL_ConvertSurface(source, &format, 0);
    
        if(!tmp) {
            throw sdl_error();
        }
    
        return tmp;
    }

    void CopyImage(const Image &source, const core::Rect &sourceRect, Image &target, const core::Point &targetPoint)
    {
        // tempRect would be modified by SDL_BlitSurface
        core::Rect tempRect(targetPoint, 0, 0);
    
        if(SDL_BlitSurface(source, &sourceRect, target, &tempRect) < 0) {
            throw sdl_error();
        }
    }

    void BlitImageScaled(const Image &source, const core::Rect &sourceRect, Image &dest, const core::Rect &targetRect)
    {
        // see BlitImage
        core::Rect tempRect(targetRect);
        if(SDL_BlitScaled(source, &sourceRect, dest, &tempRect) < 0) {
            throw sdl_error();
        }
    }

    uint32_t ExtractPixel(const Image &surface, const core::Point &point)
    {
        if(!surface) {
            throw null_surface_error();
        }

        if(!core::PointInRect(core::Rect(surface), point)) {
            throw std::invalid_argument("coord out of surface bounds");
        }
    
        const ImageLocker lock(surface);
        return core::GetPackedPixel(
            lock.Data()
            + point.y * surface.RowStride()
            + point.x * surface.PixelStride(), surface.PixelStride());
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

    SDL_PixelFormat const& ImageFormat(const Image &surface)
    {
        assert(surface->format != NULL);
        return *surface->format;
    }

    char const* ImageLocker::Data() const
    {
        return reinterpret_cast<const char*>(mImage->pixels);
    }
    
    char* ImageLocker::Data()
    {
        return reinterpret_cast<char*>(mImage->pixels);
    }

    size_t Image::RowStride() const
    {
        assert(!Null());
        return mSurface->pitch;
    }

    size_t Image::PixelStride() const
    {
        assert(!Null());
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
        return SDL_BLENDMODE_NONE;
    }
    
    void Image::SetBlendMode(Image::BlendMode mode)
    {
    }

    void Image::AttachPalette(Palette &palette)
    {
        assert(!Null());
        if(SDL_SetSurfacePalette(mSurface, &palette.asSDLPalette()) < 0) {
            throw sdl_error();
        }
    }
}

#include "surface.h"

#include <cassert>
#include <SDL.h>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <queue>
#include <cstdint>
#include <boost/circular_buffer.hpp>

#include "geometry.h"
#include "macrosota.h"

namespace
{
    
    template<class Pixel>
    void TransformBuffer(void *pixels, size_t count, const SDL_PixelFormat *pf, PixelMapper f)
    {
        Pixel *first = reinterpret_cast<Pixel *>(pixels);
        Pixel *last = first + count;
        
        while(first != last) {
            uint8_t r, g, b, a;
            SDL_GetRGBA(*first, pf, &r, &g, &b, &a);
        
            SDL_Color result = f(r, g, b, a);
            *first = SDL_MapRGBA(pf, result.r, result.g, result.b, result.a);
            first = std::next(first);
        }
    }

    template<class Pixel>
    void MapSurfaceImpl(Surface &dst, PixelMapper f)
    {
        if(dst.Null())
            return;
    
        SurfaceLocker lock(dst);

        uint8_t *pixels = reinterpret_cast<uint8_t *>(dst->pixels);
        const SDL_PixelFormat *fmt = dst->format;

        for(int y = 0; y < dst->h; ++y) {
            TransformBuffer<Pixel>(pixels, dst->w, fmt, f);
            std::advance(pixels, dst->pitch);
        }
    }
    
    void CopySurfaceColorKey(SDL_Surface *src, SDL_Surface *dst)
    {
        uint32_t colorkey;
        bool enabled = (0 == SDL_GetColorKey(src, &colorkey));

        SDL_SetColorKey(dst, enabled, colorkey);
    }

    void AddSurfaceRef(SDL_Surface *surface)
    {
        if(surface != NULL) {
            ++surface->refcount;
        }
    }

    SDL_Surface *CreateSurfaceFrom(void *pixels, int width, int height, int pitch,
                                   const SDL_PixelFormat *format)
    {
        if(pixels == NULL) {
            throw std::invalid_argument("CreateSurfaceFrom: passed NULL pixels");
        }
        if(format == NULL) {
            throw std::invalid_argument("CreateSurfaceFrom: passed NULL format");
        }
        
        uint32_t rmask = format->Rmask;
        uint32_t gmask = format->Gmask;
        uint32_t bmask = format->Bmask;
        uint32_t amask = format->Amask;
        int bpp = format->BitsPerPixel;

        return SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
    }

    SDL_Surface *CreateSurface(int width, int height, const SDL_PixelFormat *format)
    {
        if(format == NULL) {
            throw std::invalid_argument("CreateSurface: passed NULL format");
        }

        uint32_t rmask = format->Rmask;
        uint32_t gmask = format->Gmask;
        uint32_t bmask = format->Bmask;
        uint32_t amask = format->Amask;
        int bpp = format->BitsPerPixel;

        return SDL_CreateRGBSurface(NoFlags, width, height, bpp, rmask, gmask, bmask, amask);
    }

    template<class Pixel>
    void BlurHorizontal(Surface &dst, int radius)
    {
        std::vector<int> reds(dst->w, 0);
        std::vector<int> greens(dst->w, 0);
        std::vector<int> blues(dst->w, 0);

        for(int y = 0; y < dst->h; ++y) {
            Pixel *scanline = (Pixel *)((uint8_t *)dst->pixels + y * dst->pitch);
            
            for(int x = 0; x < dst->w; ++x) {
                uint8_t r, g, b;
                SDL_GetRGB(scanline[x], dst->format, &r, &g, &b);
                if(x != 0) {
                    reds[x] = reds[x-1] + r;
                    greens[x] = greens[x-1] + g;
                    blues[x] = blues[x-1] + b;
                } else {
                    reds[x] = r;
                    greens[x] = g;
                    blues[x] = b;
                }
            }

            for(int x = 0; x < dst->w; ++x) {
                int red = 0;
                int green = 0;
                int blue = 0;
                int count = 0;                
                if(x - radius >= 0) {
                    red -= reds[x - radius];
                    green -= greens[x - radius];
                    blue -= blues[x - radius];
                    count += radius;
                } else {
                    count += x;
                }
                if(x + radius < dst->w) {
                    red += reds[x + radius];
                    green += greens[x + radius];
                    blue += blues[x + radius];
                    count += radius;
                } else {
                    red += reds[dst->w - 1];
                    green += greens[dst->w - 1];
                    blue += blues[dst->w - 1];
                    count += dst->w - x;
                }
                scanline[x] = SDL_MapRGB(dst->format,
                                         red / count,
                                         green / count,
                                         blue / count);
            }
        }
    }

        template<class Pixel>
    void BlurVertical(Surface &dst, int radius)
    {
        UNUSED(dst);
        UNUSED(radius);
    }
    
    template<class Pixel>
    void BlurSurfaceImpl(Surface &dst, int radius)
    {
        SurfaceLocker lock(dst);
        if(dst.Null())
            return;

        BlurHorizontal<Pixel>(dst, radius);
        BlurVertical<Pixel>(dst, radius);
    }
    
}

SurfaceLocker::SurfaceLocker(const Surface &surface)
    : object(surface)
    , locked(false)
{
    if(!object.Null()) {
        if(SDL_MUSTLOCK(object)) {
            if(0 == SDL_LockSurface(object))
                locked = true;
        }
    }
}

SurfaceLocker::~SurfaceLocker()
{
    if(locked && !object.Null()) {
        SDL_UnlockSurface(object);
    }
}

ColorKeyLocker::ColorKeyLocker(const Surface &surface, bool enabled, uint32_t color)
    : object(surface)
{
    if(!object.Null()) {
        oldEnabled =
            (0 == SDL_GetColorKey(object, &oldColor));
        SDL_SetColorKey(object, enabled, color);
    }
}

ColorKeyLocker::~ColorKeyLocker()
{
    if(!object.Null()) {
        SDL_SetColorKey(object, oldEnabled, oldColor);
    }
}

Surface::~Surface()
{
    SDL_FreeSurface(mSurface);
}

Surface::Surface()
    : mSurface(NULL)
{
}

Surface::Surface(SDL_Surface *s)
    : mSurface(s)
{
}

Surface::Surface(const Surface &that)
    : mSurface(that.mSurface)
{
    AddSurfaceRef(mSurface);
}

Surface &Surface::operator=(SDL_Surface *s)
{
    Assign(s);
    return *this;
}

Surface &Surface::operator=(const Surface &that)
{
    AddSurfaceRef(that.mSurface);
    Assign(that.mSurface);
    return *this;
}

bool Surface::operator==(const Surface &that)
{
    return that.mSurface == mSurface;
}

void Surface::Assign(SDL_Surface *s)
{
    // SDL_FreeSurface manages refcount by itself
    // Suddenly.
    if(mSurface != NULL) {
        if(mSurface->refcount == 1) {
            std::clog << "Remove this shitty surface: "
                      << std::hex << mSurface
                      << std::endl;
        }
    }
    SDL_FreeSurface(mSurface);
    mSurface = s;
}

bool Surface::Null() const
{
    return (mSurface == NULL);
}

SDL_Surface *Surface::operator->() const
{
    return mSurface;
}

Surface::operator SDL_Surface *() const
{
    return mSurface;
}

void Surface::reset()
{
    Assign(NULL);
}

SurfaceROI::SurfaceROI(const Surface &src, const SDL_Rect *roi)
    : mReferer(NULL)
{
    if(src.Null())
        return;
    
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;

    if(roi != NULL) {
        x = roi->x;
        y = roi->y;
        width = roi->w;
        height = roi->h;
    } else {
        x = 0;
        y = 0;
        width = src->w;
        height = src->h;
    }    

    uint32_t bytesPerPixel = src->format->BytesPerPixel;
    
    uint8_t *pixels = reinterpret_cast<uint8_t*>(src->pixels)
        + y * src->pitch
        + x * bytesPerPixel;
    
    mSurface = CreateSurfaceFrom(pixels, width, height, src->pitch, src->format);    
    ThrowSDLError(mSurface);
    
    CopySurfaceColorKey(src, mSurface);
    
    mReferer = src;
    AddSurfaceRef(mReferer);
}

SurfaceROI::~SurfaceROI()
{
    SDL_FreeSurface(mReferer);
}

Surface CopySurfaceFormat(const Surface &src, int width, int height)
{
    if(src.Null())
        return Surface();
    
    Surface dst = CreateSurface(width, height, src->format);
    if(dst.Null()) {
        throw std::runtime_error(SDL_GetError());
    }

    CopySurfaceColorKey(src, dst);

    return dst;
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    ThrowSDLError(
        SDL_BlitSurface(src, srcrect, dst, dstrect));
}

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, uint32_t color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    uint8_t r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render.get(), r, g, b, a);
    SDL_RenderDrawRect(render.get(), dstrect);
}

void FillFrame(Surface &dst, const SDL_Rect *dstrect, uint32_t color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    uint8_t r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render.get(), r, g, b, a);
    SDL_RenderFillRect(render.get(), dstrect);
}

SDL_Rect SurfaceBounds(const Surface &src)
{
    return (src.Null()
            ? MakeEmptyRect()
            : MakeRect(src->w, src->h));
}

bool HasPalette(const Surface &surface)
{
    return !surface.Null()
        && surface->format != NULL
        && surface->format->BitsPerPixel == 8;
}

void MapSurface(Surface &dst, PixelMapper f)
{
    switch(dst->format->BytesPerPixel) {
    case 1:
        MapSurfaceImpl<uint8_t>(dst, f);
        break;
    case 2:
        MapSurfaceImpl<uint16_t>(dst, f);
        break;
    case 3:
        // TODO implement me
        break;
    case 4:
        MapSurfaceImpl<uint32_t>(dst, f);
        break;
    default:
        throw std::runtime_error("Unsupported BPP");
    }
}

void BlurSurface(Surface &dst, int radius)
{
    switch(dst->format->BytesPerPixel) {
    case 1:
        BlurSurfaceImpl<uint8_t>(dst, radius);
        break;
    case 2:
        BlurSurfaceImpl<uint16_t>(dst, radius);
        break;
    case 3:
        // TODO implement me
        break;
    case 4:
        BlurSurfaceImpl<uint32_t>(dst, radius);
        break;
    default:
        throw std::runtime_error("Unsupported BPP");
    }
}

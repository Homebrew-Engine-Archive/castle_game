#include "surface.h"

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include <SDL.h>

#include <boost/current_function.hpp>

#include <game/sdl_utils.h>
#include <game/make_unique.h>

namespace
{

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
    
    void MapBuffer(char *data, size_t size, const SDL_PixelFormat *format, SDL_Color func(uint8_t, uint8_t, uint8_t, uint8_t))
    {
        const char *end = data + size * format->BytesPerPixel;
        
        while(data != end) {
            uint32_t pixel = GetPackedPixel(data, format->BytesPerPixel);

            uint8_t r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
        
            SDL_Color result = func(r, g, b, a);

            pixel = SDL_MapRGBA(format, result.r, result.g, result.b, result.a);
            SetPackedPixel(data, pixel, format->BytesPerPixel);
            
            data += format->BytesPerPixel;
        }
    }
    
    void AddSurfaceRef(SDL_Surface *surface)
    {
        if(surface != nullptr) {
            ++surface->refcount;
        }
    }

    template<class Pixel>
    void BlurHorizontal(Surface &dst, int radius)
    {
        std::vector<int> reds(dst->w, 0);
        std::vector<int> greens(dst->w, 0);
        std::vector<int> blues(dst->w, 0);

        for(int y = 0; y < dst->h; ++y) {
            Pixel *scanline = reinterpret_cast<Pixel*>(reinterpret_cast<char*>(dst->pixels) + y * dst->pitch);
            
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
    void BlurVertical(Surface &, int)
    {
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
    : mObject(surface)
    , mLocked(false)
{
    if(!mObject.Null()) {
        if(SDL_MUSTLOCK(mObject)) {
            if(0 == SDL_LockSurface(mObject))
                mLocked = true;
        }
    }
}

SurfaceLocker::~SurfaceLocker()
{
    if(mLocked && !mObject.Null()) {
        SDL_UnlockSurface(mObject);
    }
}

Surface::~Surface()
{
    SDL_FreeSurface(mSurface);
}

Surface::Surface()
    : mSurface(nullptr)
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
    SDL_FreeSurface(mSurface);
    mSurface = s;
}

bool Surface::Null() const
{
    return (mSurface == nullptr);
}

SDL_Surface *Surface::operator->() const
{
    return mSurface;
}

Surface::operator SDL_Surface *() const
{
    return mSurface;
}

bool Surface::operator!() const
{
    return Null();
}

void Surface::reset()
{
    Assign(nullptr);
}

SurfaceROI::SurfaceROI(Surface &src, const SDL_Rect *roi)
{
    if(!src) {
        return;
    }
    
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    if(roi != nullptr) {
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

    const int bytesPerPixel = src->format->BytesPerPixel;
    
    char *pixels = GetPixels(src)
        + y * src->pitch
        + x * bytesPerPixel;
    
    Surface tmp = CreateSurfaceFrom(pixels, width, height, src->pitch, src->format);
    if(!tmp) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
    
    CopyColorKey(src, tmp);

    mSurface = tmp;
    AddSurfaceRef(mSurface);
    
    mReferer = src;
}

void CopyColorKey(SDL_Surface *src, SDL_Surface *dst)
{
    uint32_t colorkey = 0;
    if(SDL_GetColorKey(src, &colorkey) == 0) {
        if(SDL_SetColorKey(dst, SDL_TRUE, colorkey) < 0) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
    }
}

Surface CreateSurface(int width, int height, const SDL_PixelFormat *format)
{
    if(format == nullptr) {
        throw std::invalid_argument("CreateSurface: passed nullptr format");
    }

    uint32_t rmask = format->Rmask;
    uint32_t gmask = format->Gmask;
    uint32_t bmask = format->Bmask;
    uint32_t amask = format->Amask;
    int bpp = format->BitsPerPixel;

    return SDL_CreateRGBSurface(NoFlags, width, height, bpp, rmask, gmask, bmask, amask);
}

Surface CreateSurface(int width, int height, int format)
{
    uint32_t rmask = 0;
    uint32_t gmask = 0;
    uint32_t bmask = 0;
    uint32_t amask = 0;
    int bpp = 0;

    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
    
    return SDL_CreateRGBSurface(NoFlags, width, height, bpp, rmask, gmask, bmask, amask);
}

Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat *format)
{
    if(pixels == nullptr) {
        std::ostringstream oss;
        oss << BOOST_CURRENT_FUNCTION << ": passed nullptr pixels";
        throw std::invalid_argument(oss.str());
    }
    
    if(format == nullptr) {
        std::ostringstream oss;
        oss << BOOST_CURRENT_FUNCTION << ": passed nullptr format";
        throw std::invalid_argument(oss.str());
    }
        
    uint32_t rmask = format->Rmask;
    uint32_t gmask = format->Gmask;
    uint32_t bmask = format->Bmask;
    uint32_t amask = format->Amask;
    int bpp = format->BitsPerPixel;

    return SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
}

Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, int format)
{
    if(pixels == nullptr) {
        std::ostringstream oss;
        oss << BOOST_CURRENT_FUNCTION << ": passed nullptr pixels";
        throw std::invalid_argument(oss.str());
    }
        
    uint32_t rmask = 0;
    uint32_t gmask = 0;
    uint32_t bmask = 0;
    uint32_t amask = 0;
    int bpp = 0;

    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }

    return SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    if(SDL_BlitSurface(src, srcrect, dst, dstrect) < 0) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
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
        && surface->format != nullptr
        && surface->format->BitsPerPixel == 8;
}

void MapSurface(Surface &dst, SDL_Color func(uint8_t, uint8_t, uint8_t, uint8_t))
{
    if(!dst) {
        return;
    }
    
    const SurfaceLocker lock(dst);

    char *data = GetPixels(dst);
    const SDL_PixelFormat *fmt = dst->format;

    for(int y = 0; y < dst->h; ++y) {
        MapBuffer(data, dst->w, fmt, func);
        data += dst->pitch;
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
    case 4:
        BlurSurfaceImpl<uint32_t>(dst, radius);
        break;
    case 3:
        // TODO implement me
        /* fallthrough */
    default:
        throw std::runtime_error("Unsupported BPP");
    }
}

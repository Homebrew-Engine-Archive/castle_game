#include "surface.h"
#include "geometry.h"
#include "macrosota.h"
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include "SDL.h"
#include <iostream>

namespace
{
    
    template<class Pixel>
    void TransformBuffer(void *pixels, size_t count, const SDL_PixelFormat *pf, PixelMapper f)
    {
        Pixel *first = reinterpret_cast<Pixel *>(pixels);
        Pixel *last = first + count;
        
        while(first != last) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(*first, pf, &r, &g, &b, &a);
        
            SDL_Color result = f(r, g, b, a);
            *first = SDL_MapRGBA(pf, result.r, result.g, result.b, result.a);
            first = std::next(first);
        }
    }

    template<class Pixel>
    void TransformImpl(Surface &dst, PixelMapper f)
    {
        if(dst.Null())
            return;
    
        SurfaceLocker lock(dst);

        Uint8 *pixels = reinterpret_cast<Uint8 *>(dst->pixels);
        const SDL_PixelFormat *fmt = dst->format;

        for(int y = 0; y < dst->h; ++y) {
            TransformBuffer<Pixel>(pixels, dst->w, fmt, f);
            std::advance(pixels, dst->pitch);
        }
    }
    
    void CopySurfaceColorKey(SDL_Surface *src, SDL_Surface *dst)
    {
        Uint32 colorkey;
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
        
        Uint32 rmask = format->Rmask;
        Uint32 gmask = format->Gmask;
        Uint32 bmask = format->Bmask;
        Uint32 amask = format->Amask;
        int bpp = format->BitsPerPixel;

        return SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
    }

    SDL_Surface *CreateSurface(int width, int height, const SDL_PixelFormat *format)
    {
        if(format == NULL) {
            throw std::invalid_argument("CreateSurface: passed NULL format");
        }

        Uint32 rmask = format->Rmask;
        Uint32 gmask = format->Gmask;
        Uint32 bmask = format->Bmask;
        Uint32 amask = format->Amask;
        int bpp = format->BitsPerPixel;

        return SDL_CreateRGBSurface(NO_FLAGS, width, height, bpp, rmask, gmask, bmask, amask);
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

ColorKeyLocker::ColorKeyLocker(const Surface &surface, bool enabled, Uint32 color)
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
    SDL_FreeSurface(m_surface);
}

Surface::Surface()
    : m_surface(NULL)
{
}

Surface::Surface(SDL_Surface *s)
    : m_surface(s)
{
}

Surface::Surface(const Surface &that)
    : m_surface(that.m_surface)
{
    AddSurfaceRef(m_surface);
}

Surface &Surface::operator=(SDL_Surface *s)
{
    Assign(s);
    return *this;
}

Surface &Surface::operator=(const Surface &that)
{
    AddSurfaceRef(that.m_surface);
    Assign(that.m_surface);
    return *this;
}

bool Surface::operator==(const Surface &that)
{
    return that.m_surface == m_surface;
}

void Surface::Assign(SDL_Surface *s)
{
    // SDL_FreeSurface manages refcount by itself
    // Suddenly.
    if(m_surface != NULL) {
        if(m_surface->refcount == 1) {
            std::clog << "Remove this shitty surface: " << std::hex << m_surface << std::endl;
        }
    }
    SDL_FreeSurface(m_surface);
    m_surface = s;
}

bool Surface::Null() const
{
    return (m_surface == NULL);
}

SDL_Surface *Surface::operator->() const
{
    return m_surface;
}

Surface::operator SDL_Surface *() const
{
    return m_surface;
}

void Surface::reset()
{
    Assign(NULL);
}

SurfaceROI::SurfaceROI(const Surface &src, const SDL_Rect *roi)
    : m_referer(NULL)
{
    if(src.Null())
        return;
    
    Uint32 x;
    Uint32 y;
    Uint32 width;
    Uint32 height;

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
    

    Uint32 bytesPerPixel = src->format->BytesPerPixel;
    
    Uint8 *pixels = reinterpret_cast<Uint8*>(src->pixels)
        + y * src->pitch
        + x * bytesPerPixel;
    
    m_surface = CreateSurfaceFrom(pixels, width, height, src->pitch, src->format);    
    ThrowSDLError(m_surface);
    
    CopySurfaceColorKey(src, m_surface);
    
    m_referer = src;
    AddSurfaceRef(m_referer);
}

SurfaceROI::~SurfaceROI()
{
    SDL_FreeSurface(m_referer);
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

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    Uint8 r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render.get(), r, g, b, a);
    SDL_RenderDrawRect(render.get(), dstrect);
}

void FillFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    Uint8 r, g, b, a;
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
        TransformImpl<Uint8>(dst, f);
        break;
    case 2:
        TransformImpl<Uint16>(dst, f);
        break;
    case 3:
        // TODO implement me
        break;
    case 4:
        TransformImpl<Uint32>(dst, f);
        break;
    default:
        throw std::runtime_error("Unsupported BPP");
    }
}
    

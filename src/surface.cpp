#include "surface.h"

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
    AddRef(m_surface);
}

Surface &Surface::operator=(SDL_Surface *s)
{
    Assign(s);
    return *this;
}

Surface &Surface::operator=(const Surface &that)
{
    AddRef(that.m_surface);
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
    SDL_FreeSurface(m_surface);
    m_surface = s;
}

void Surface::AddRef(SDL_Surface *surf)
{
    if(surf != NULL)
        ++surf->refcount;
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

Surface CopySurface(const Surface &src, const SDL_Rect *srcrect)
{
    Uint32 width = (srcrect == NULL
                ? src->w
                : srcrect->w);
    Uint32 height = (srcrect == NULL
                     ? src->h
                     : srcrect->h);
    Uint32 depth = src->format->BitsPerPixel;
    Uint32 rmask = src->format->Rmask;
    Uint32 gmask = src->format->Gmask;
    Uint32 bmask = src->format->Bmask;
    Uint32 amask = src->format->Amask;
    
    Surface dst = SDL_CreateRGBSurface(
        0, width, height, depth,
        rmask, gmask, bmask, amask);
    
    Uint32 key;
    bool enabled = 
        (0 == SDL_GetColorKey(src, &key));
    
    ColorKeyLocker lock(src, SDL_FALSE, key);
    BlitSurface(src, srcrect, dst, NULL);
    SDL_SetColorKey(dst, enabled, key);
    
    return dst;
}

Surface SubSurface(Surface &src, const SDL_Rect *rect)
{
    Uint32 x = (rect == NULL ? 0 : rect->x);
    Uint32 y = (rect == NULL ? 0 : rect->y);
    Uint32 width = (rect == NULL ? src->w : rect->w);
    Uint32 height = (rect == NULL ? src->h : rect->h);
    Uint32 depth = src->format->BitsPerPixel;
    Uint32 rmask = src->format->Rmask;
    Uint32 gmask = src->format->Gmask;
    Uint32 bmask = src->format->Bmask;
    Uint32 amask = src->format->Amask;
    Uint32 bytesPerPixel = src->format->BytesPerPixel;
    
    Uint8 *pixels = (Uint8 *)src->pixels
        + y * src->pitch
        + x * bytesPerPixel;
    
    Surface dst = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, depth, src->pitch,
        rmask, gmask, bmask, amask);
    
    Uint32 colorkey;
    if(0 == SDL_GetColorKey(src, &colorkey)) {
        SDL_SetColorKey(dst, SDL_TRUE, colorkey);
    }
    
    return dst;
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    if(SDL_BlitSurface(src, srcrect, dst, dstrect)) {
        throw std::runtime_error(SDL_GetError());
    }
}

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_Renderer *render = SDL_CreateSoftwareRenderer(dst);
    SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

    Uint8 r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render, r, g, b, a);
    SDL_RenderDrawRect(render, dstrect);
    SDL_DestroyRenderer(render);
}

void FillFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_Renderer *render = SDL_CreateSoftwareRenderer(dst);
    SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

    Uint8 r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render, r, g, b, a);
    SDL_RenderFillRect(render, dstrect);
    SDL_DestroyRenderer(render);
}

SDL_Rect AlignRect(const SDL_Rect &src, const SDL_Rect &dst, double x, double y)
{
    int xcenter = dst.x + (dst.w / 2);
    int ycenter = dst.y + (dst.h / 2);

    int xspace = std::max(0, dst.w - src.w);
    int yspace = std::max(0, dst.h - src.h);

    int xpos = x * xspace / 2;
    int ypos = y * yspace / 2;

    SDL_Rect rect;
    rect.x = xcenter + xpos - (src.w / 2);
    rect.y = ycenter + ypos - (src.h / 2);
    rect.w = src.w;
    rect.h = src.h;
    return rect;
}

SDL_Rect PadIn(const SDL_Rect &src, int pad)
{
    if((src.w >= (2 * pad)) && (src.h >= (2 * pad))) {
        return MakeRect(
            src.x + pad,
            src.y + pad,
            src.w - 2 * pad,
            src.h - 2 * pad);
    }

    return src;
}

SDL_Rect SurfaceBounds(const Surface &src)
{
    return (src.Null()
            ? MakeEmptyRect()
            : MakeRect(src->w, src->h));
}

SDL_Rect MakeRect(int x, int y, int w, int h)
{
    SDL_Rect r = {x, y, w, h};
    return r;
}

SDL_Rect MakeRect(int w, int h)
{
    SDL_Rect r = {0, 0, w, h};
    return r;
}

SDL_Rect MakeEmptyRect()
{
    SDL_Rect r = {0, 0, 0, 0};
    return r;
}

bool HasPalette(Surface surface)
{
    return !surface.Null()
        && surface->format != NULL
        && surface->format->BitsPerPixel == 8;
}

NAMESPACE_BEGIN(std)

std::ostream &operator<<(std::ostream &out, const SDL_Rect &R)
{
    out << R.w << 'x' << R.h;
    if(R.x >= 0)
        out << '+';
    out << R.x;
    if(R.y >= 0)
        out << '+';
    out << R.y;
    return out;
}

NAMESPACE_END(std)

#include "renderer.h"

#include <memory>
#include <string>

#include <cassert>

#include <game/clamp.h>
#include <game/make_unique.h>

#include <game/rect.h>
#include <game/point.h>
#include <game/color.h>
#include <game/gm1palette.h>
#include <game/sdl_error.h>
#include <game/sdl_utils.h>
#include <game/collection.h>
#include <game/surface_drawing.h>
#include <game/fontmanager.h>

namespace
{
    std::string GetBlendModeName(SDL_BlendMode mode)
    {
#define CASE(x) case x: return #x
        switch(mode) {
            CASE(SDL_BLENDMODE_BLEND);
            CASE(SDL_BLENDMODE_ADD);
            CASE(SDL_BLENDMODE_MOD);
            CASE(SDL_BLENDMODE_NONE);
        default:
            return "SDL_BLENDMODE_UNKNOWN";
        }
#undef CASE
    }
    
    const uint32_t ScreenPixelFormat = SDL_PIXELFORMAT_RGB888;
    const int WindowWidth = 1024;
    const int WindowHeight = 768;

    const int WindowXPos = SDL_WINDOWPOS_UNDEFINED;
    const int WindowYPos = SDL_WINDOWPOS_UNDEFINED;

    const char *WindowTitle = "Castle game";

    const int WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    const int RendererIndex = -1;
    const int RendererFlags = SDL_RENDERER_ACCELERATED;

    const int MaxAlpha = 0xFF;
    
    const int MinScreenWidth = 0;
    const int MinScreenHeight = 0;
    
    // TODO sync with driver's texture max width and height
    const int MaxScreenWidth = 4096;
    const int MaxScreenHeight = 4096;

    int AdjustWidth(int width)
    {
        return core::Clamp(width, MinScreenWidth, MaxScreenWidth);
    }

    int AdjustHeight(int height)
    {
        return core::Clamp(height, MinScreenHeight, MaxScreenHeight);
    }
}

namespace Render
{
    Renderer::~Renderer() = default;
    
    Renderer::Renderer()
        : mScreenWidth(WindowWidth)
        , mScreenHeight(WindowHeight)
        , mScreenFormat(ScreenPixelFormat)
        , mScreenTexture(nullptr)
        , mScreenSurface(nullptr)
        , mBoundPalette()
        , mBoundSurface(nullptr)
        , mBoundAlphaMap(nullptr)
        , mClipStack()
        , mAlphaStack()
        , mAlpha(1.0f)
        , mFontManager(std::make_unique<FontManager>())
    {
        mWindow.reset(
            SDL_CreateWindow(WindowTitle,
                             WindowXPos,
                             WindowYPos,
                             mScreenWidth,
                             mScreenHeight,
                             WindowFlags));
        
        if(!mWindow) {
            throw sdl_error();
        }
        
        mRenderer.reset(
            SDL_CreateRenderer(mWindow.get(),
                               RendererIndex,
                               RendererFlags));
        if(!mRenderer) {
            throw sdl_error();
        }
    }

    FontManager& Renderer::GetFontManager()
    {
        return *mFontManager;
    }
    
    FontManager const& Renderer::GetFontManager() const
    {
        return *mFontManager;
    }

    void Renderer::CreateScreenTexture(int width, int height, int format)
    {
        if((mScreenTexture) && (width == mScreenWidth) && (height == mScreenHeight) && (format == mScreenFormat)) {
            return;
        }

        TexturePtr temp(
            SDL_CreateTexture(
                mRenderer.get(),
                format,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height));

        // Let sdl check the size and the format for us
        if(!temp) {
            throw sdl_error();
        }

        mScreenTexture = std::move(temp);
        mScreenFormat = format;
        mScreenWidth = width;
        mScreenHeight = height;
    }

    void Renderer::CreateScreenSurface(int width, int height)
    {
        const Surface temp = CreateSurface(width, height, mScreenFormat);
        if(!temp) {
            throw sdl_error();
        }

        if(SDL_SetSurfaceBlendMode(temp, SDL_BLENDMODE_NONE) < 0) {
            throw sdl_error();
        }
        
        mScreenSurface = temp;
    }

    void Renderer::CreatePrimitiveRenderer()
    {
        RendererPtr renderer(SDL_CreateSoftwareRenderer(mScreenSurface));
        if(!renderer) {
            throw sdl_error();
        }

        mPrimitiveRenderer = std::move(renderer);
    }
    
    bool Renderer::ReallocationRequired(int width, int height, int format) const
    {
        return (width != mScreenWidth) || (height != mScreenHeight) || (format != mScreenFormat);
    }
    
    void Renderer::BeginFrame()
    {
        if(!mScreenTexture) {
            CreateScreenTexture(mScreenWidth, mScreenHeight, mScreenFormat);
        }

        if(!mScreenSurface) {
            CreateScreenSurface(mScreenWidth, mScreenHeight);
        }

        if(!mPrimitiveRenderer) {
            CreatePrimitiveRenderer();
        }

        SDL_SetClipRect(mScreenSurface, NULL);
        SDL_FillRect(mScreenSurface, NULL, 0);

        mClipStack.clear();
        mScreenRectStack.clear();
        mAlphaStack.clear();
        mAlpha = MaxAlpha;
    }
    
    void Renderer::EndFrame()
    {
        if(!mScreenSurface.Null()) {
            const SurfaceLocker lock(mScreenSurface);
            
            if(SDL_UpdateTexture(mScreenTexture.get(), NULL, SurfaceData(mScreenSurface), SurfaceRowStride(mScreenSurface)) < 0) {
                throw sdl_error();
            }

            const Rect textureRect(mScreenSurface);
            if(SDL_RenderCopy(mRenderer.get(), mScreenTexture.get(), &textureRect, &textureRect) < 0) {
                throw sdl_error();
            }
        }
        
        SDL_RenderPresent(mRenderer.get());
    }

    const Point Renderer::GetMaxOutputSize() const
    {
        return GetOutputSize();
    }
    
    const Point Renderer::GetOutputSize() const
    {
        Point size;
        
        if(SDL_GetRendererOutputSize(mRenderer.get(), &size.x, &size.y) < 0) {
            throw sdl_error();
        }
        
        return size;
    }

    const Rect Renderer::GetScreenClipRect() const
    {
        return (mClipStack.empty()
                ? Rect(mScreenSurface)
                : mClipStack.back());
    }
    
    const Rect Renderer::GetScreenRect() const
    {
        return (mScreenRectStack.empty()
                ? Rect(mScreenSurface)
                : Rect(mScreenRectStack.back().w,
                       mScreenRectStack.back().h));
    }

    void Renderer::Clip(const Rect &clipArea)
    {
        if(RectEmpty(clipArea)) {
            mClipStack.push_back(Rect());
            mScreenRectStack.push_back(Rect());
            return;
        }
        
        const Rect unclippedArea = UnclipRect(clipArea);
        const Rect oldClipArea = GetScreenClipRect();
        const Rect newClipArea = Intersection(oldClipArea, unclippedArea);
        mClipStack.push_back(newClipArea);
        mScreenRectStack.push_back(unclippedArea);
    }

    void Renderer::Unclip()
    {
        if(mClipStack.empty()) {
            throw std::runtime_error("clip stack is empty");
        }

        mScreenRectStack.pop_back();
        mClipStack.pop_back();
    }

    void Renderer::Alpha(int alpha)
    {
        mAlphaStack.push_back(mAlpha);
        mAlpha = (alpha * mAlpha) / MaxAlpha;
    }

    void Renderer::Unalpha()
    {
        mAlpha = mAlphaStack.back();
        mAlphaStack.pop_back();
    }
    
    const Rect Renderer::UnclipRect(const Rect &rect) const
    {
        return Translated(rect, TopLeft(
                              (mScreenRectStack.empty()
                               ? Rect()
                               : mScreenRectStack.back())));
    }

    const Point Renderer::ClipPoint(const Point &point) const
    {
        return point - TopLeft(GetScreenClipRect());
    }
    
    void Renderer::SetScreenMode(int width, int height, int format)
    {
        if(ReallocationRequired(width, height, format)) {
            mPrimitiveRenderer = nullptr;
            mScreenSurface = nullptr;
            mScreenTexture = nullptr;
            mScreenWidth = width;
            mScreenHeight = height;
            mScreenFormat = format;
        }
    }

    void Renderer::SetScreenFormat(int format)
    {
        SetScreenMode(mScreenWidth, mScreenHeight, format);
    }
    
    void Renderer::SetScreenSize(int width, int height)
    {
        // Cutting up and down texture height and width
        const int newWidth = AdjustWidth(width);
        const int newHeight = AdjustHeight(height);

        SetScreenMode(newWidth, newHeight, mScreenFormat);
    }
    
    void Renderer::BindSurface(const Surface &surface)
    {
        mBoundSurface = surface;
    }
    
    void Renderer::BindPalette(const GM1::Palette &palette)
    {
        mBoundPalette = palette;
    }
    
    void Renderer::BindAlphaMap(const Surface &surface)
    {
        mBoundAlphaMap = surface;
    }

    void Renderer::UnbindSurface()
    {
        mBoundSurface = nullptr;
    }

    void Renderer::UnbindPalette()
    {
        mBoundPalette = GM1::Palette();
    }

    void Renderer::UnbindAlphaMap()
    {
        mBoundAlphaMap = nullptr;
    }

    void Renderer::FillRhombus(const Rect &rect, const Color &color)
    {        
        const Rect unclippedRect = UnclipRect(rect);
        const Rect clipRect = GetScreenClipRect();
        if(Intersects(unclippedRect, clipRect)) {
            SDL_RenderSetClipRect(mPrimitiveRenderer.get(), &clipRect);
            Render::FillRhombus(*mPrimitiveRenderer, unclippedRect, color);
        }
    }

    void Renderer::DrawRhombus(const Rect &rect, const Color &color)
    {        
        const Rect unclippedRect = UnclipRect(rect);
        const Rect clipRect = GetScreenClipRect();
        if(Intersects(unclippedRect, clipRect)) {
            SDL_RenderSetClipRect(mPrimitiveRenderer.get(), &clipRect);
            Render::DrawRhombus(*mPrimitiveRenderer, unclippedRect, color);
        }
    }

    void Renderer::FillFrame(const Rect &rect, const Color &color)
    {        
        const Rect unclippedRect = UnclipRect(rect);
        const Rect clipRect = GetScreenClipRect();
        if(Intersects(unclippedRect, clipRect)) {
            SDL_RenderSetClipRect(mPrimitiveRenderer.get(), &clipRect);
            Render::FillFrame(*mPrimitiveRenderer, unclippedRect, color);
        }
    }

    void Renderer::DrawFrame(const Rect &rect, const Color &color)
    {
        const Rect unclippedRect = UnclipRect(rect);
        const Rect clipRect = GetScreenClipRect();
        if(Intersects(unclippedRect, clipRect)) {
            SDL_RenderSetClipRect(mPrimitiveRenderer.get(), &clipRect);
            Render::DrawFrame(*mPrimitiveRenderer, unclippedRect, color);
        }
    }
    
    void Renderer::Blit(const Rect &textureSubRect, const Rect &screenSubRect)
    {
        /** Correct blend mode before blit is crucial **/
        SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
        
        if(SDL_ISPIXELFORMAT_ALPHA(SurfaceFormat(mBoundSurface).format)) {
            blendMode = SDL_BLENDMODE_BLEND;
        } else if(mAlpha != MaxAlpha) {
            blendMode = SDL_BLENDMODE_BLEND;
        }

        SDL_SetSurfaceBlendMode(mBoundSurface, blendMode);
        SDL_SetSurfaceAlphaMod(mBoundSurface, mAlpha);

        if(SDL_ISPIXELFORMAT_INDEXED(SurfaceFormat(mBoundSurface).format)) {
            SDL_SetSurfacePalette(mBoundSurface, &mBoundPalette.asSDLPalette());
        }

        /** will be modified by BlitSurface **/
        Rect unclippedRect = UnclipRect(screenSubRect);
        const Rect clipRect = GetScreenClipRect();
        SDL_SetClipRect(mScreenSurface, &clipRect);

        if(Intersects(unclippedRect, clipRect)) {
            try {
                if(SDL_BlitSurface(mBoundSurface, &textureSubRect, mScreenSurface, &unclippedRect) < 0) {
                    throw sdl_error();
                }
            } catch(const std::exception &error) {
                std::cerr << "Blit surface failed: " << error.what() << std::endl
                          << "Source rect: "         << textureSubRect << std::endl
                          << "Clipped dest rect: "   << screenSubRect << std::endl
                          << "Unclipped dest rect: " << unclippedRect << std::endl
                          << "Clip rect: "           << clipRect << std::endl
                          << "Source format: "       << SDL_GetPixelFormatName(SurfaceFormat(mBoundSurface).format) << std::endl
                          << "Destination format: "  << SDL_GetPixelFormatName(SurfaceFormat(mScreenSurface).format) << std::endl
                          << "Alpha: "               << mAlpha << std::endl
                          << "Source blend mode: "   << GetBlendModeName(GetSurfaceBlendMode(mBoundSurface)) << std::endl;       
                throw;
            }
        }
    }

    void Renderer::BlitTiled(const Rect &textureSubrect, const Rect &scrennSubRect)
    {
        
    }

    void Renderer::BlitScaled(const Rect &textureSubRect, const Rect &screenSubRect)
    {

    }

} // namespace Render

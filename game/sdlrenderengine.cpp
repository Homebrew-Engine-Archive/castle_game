#include "sdlrenderengine.h"

#include <stdexcept>
#include <vector>

#include <SDL2_gfxPrimitives.h>
#include <SDL.h>

#include <game/line.h>
#include <game/surface.h>
#include <game/point.h>
#include <game/size.h>
#include <game/rect.h>
#include <game/color.h>

namespace
{
    const uint32_t WindowPixelFormat = SDL_PIXELFORMAT_RGB888;
    const int WindowWidth = 1024;
    const int WindowHeight = 768;

    const int WindowXPos = SDL_WINDOWPOS_UNDEFINED;
    const int WindowYPos = SDL_WINDOWPOS_UNDEFINED;

    const char *WindowTitle = "Castle game";

    const uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;

    const int RendererIndex = -1;
    const int RendererFlags = SDL_RENDERER_ACCELERATED;

    const OutputMode DetectOutputMode(SDL_Window &window)
    {
        int width;
        int height;
        SDL_GetWindowSize(&window, &width, &height);
        
        const uint32_t format = SDL_GetWindowPixelFormat(&window);
        return OutputMode(width, height, format);
    }
}

namespace Render
{
    SDLRenderEngine::~SDLRenderEngine() = default;
    
    SDLRenderEngine::SDLRenderEngine()
        : mRenderer(nullptr)
        , mWindow(nullptr)
        , mOutputMode()
        , mClipRect()
        , mOpacityMod()
    {
        mWindow.reset(
            SDL_CreateWindow(WindowTitle,
                             WindowXPos,
                             WindowYPos,
                             mOutputMode.Width(),
                             mOutputMode.Height(),
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

    void SDLRenderEngine::SetOpacityMod(int opacity)
    {
        mOpacityMod = opacity;
    }
    
    void SDLRenderEngine::BeginFrame()
    {
        mOutputMode = DetectOutputMode(*mWindow);
    }
    
    void SDLRenderEngine::EndFrame()
    {
        SDL_RenderPresent(mRenderer.get());
    }
    
    void SDLRenderEngine::SetOutputMode(const OutputMode &mode)
    {
        const OutputMode actual = DetectOutputMode(*mWindow);
        SDL_RenderSetScale(mRenderer.get(),
                           static_cast<float>(actual.Width()) / mode.Width(),
                           static_cast<float>(actual.Height()) / mode.Height());
        
        mOutputMode = mode;
    }
    
    const OutputMode SDLRenderEngine::GetOutputMode() const
    {
        return mOutputMode;
    }
        
    const core::Size SDLRenderEngine::GetMaxOutputSize() const
    {
        return core::Size(mOutputMode.Width(), mOutputMode.Height());
    }

    void SDLRenderEngine::UpdateClipRect(const core::Rect &rect)
    {
        const int code = SDL_RenderSetClipRect(mRenderer.get(), &rect);
        throw_sdl_error(code);
    }

    void SDLRenderEngine::UpdateDrawColor(const core::Color &color)
    {
        SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
        if(color.a == 0xff) {
            blendMode = SDL_BLENDMODE_NONE;
        }
        
        {
            const int code = SDL_SetRenderDrawBlendMode(mRenderer.get(), blendMode);
            throw_sdl_error(code);
        }

        {
            const int code = SDL_SetRenderDrawColor(mRenderer.get(), color.r, color.g, color.b, color.a);
            throw_sdl_error(code);
        }
    }
    
    void SDLRenderEngine::DrawPoints(const core::Point *points, size_t count, const core::Color &color)
    {
        UpdateClipRect(mClipRect);
        UpdateDrawColor(color);
        SDL_RenderDrawPoints(mRenderer.get(), points, count);
    }
    
    void SDLRenderEngine::DrawRects(const core::Rect *rects, size_t count, const core::Color &color, DrawMode mode)
    {
        UpdateClipRect(mClipRect);
        UpdateDrawColor(color);
        switch(mode) {
        case DrawMode::Outline:
            {
                const int code = SDL_RenderDrawRects(mRenderer.get(), rects, count);
                throw_sdl_error(code);
            }
            return;
        case DrawMode::Filled:
            {
                const int code = SDL_RenderFillRects(mRenderer.get(), rects, count);
                throw_sdl_error(code);
            }
            return;
        default:
            throw wrong_draw_mode();
        }
    }
    
    void SDLRenderEngine::DrawLines(const core::Line *lines, size_t count, const core::Color &color)
    {
        UpdateClipRect(mClipRect);
        UpdateDrawColor(color);
        
        for(size_t i = 0; i < count; ++i) {
            const auto x1 = lines[i].p1.x;
            const auto y1 = lines[i].p1.y;
            const auto x2 = lines[i].p2.x;
            const auto y2 = lines[i].p2.y;
            const int code = SDL_RenderDrawLine(mRenderer.get(), x1, y1, x2, y2);
            throw_sdl_error(code);
        }
    }

    void SDLRenderEngine::DrawPolygon(const core::Point *points, size_t count, const core::Color &color, DrawMode mode)
    {
        if(count == 0) {
            return;
        }
        
        UpdateClipRect(mClipRect);
        UpdateDrawColor(color);
        
        const size_t polycount = count + 1;
        std::vector<Sint16> xs(polycount);
        std::vector<Sint16> ys(polycount);
        
        for(size_t i = 0; i < count; ++i) {
            xs[i] = points[i].x;
            ys[i] = points[i].y;
        }
        xs[count] = points[0].x;
        ys[count] = points[0].y;
        
        switch(mode) {
        case DrawMode::Filled:
            filledPolygonRGBA(mRenderer.get(), xs.data(), ys.data(), polycount, color.r, color.g, color.b, color.a);
            return;
        case DrawMode::Outline:
            polygonRGBA(mRenderer.get(), xs.data(), ys.data(), polycount, color.r, color.g, color.b, color.a);
            return;
        default:
            throw wrong_draw_mode();
        }
    }
    
    void SDLRenderEngine::DrawSurface(const Surface &image, const core::Rect &textureRect, const core::Rect &screenRect)
    {
        if(Intersects(screenRect, mClipRect)) {
            UpdateClipRect(mClipRect);
            TexturePtr texture(
                SDL_CreateTextureFromSurface(mRenderer.get(), image));
            SDL_SetTextureAlphaMod(texture.get(), mOpacityMod);
            SDL_RenderCopy(mRenderer.get(), texture.get(), &textureRect, &screenRect);
        }
    }

    void SDLRenderEngine::DrawSurfaceTiled(const Surface &image, const core::Rect &source, const core::Rect &target)
    {
    }
    
    void SDLRenderEngine::DrawSurfaceScaled(const Surface &image, const core::Rect &source, const core::Rect &target)
    {
    }
    
    void SDLRenderEngine::ClipRect(const core::Rect &rect)
    {
        mClipRect = rect;
    }
    
    void SDLRenderEngine::ClearOutput(const core::Color &color)
    {
        SDL_RenderSetClipRect(mRenderer.get(), NULL);
        SDL_SetRenderDrawColor(mRenderer.get(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(mRenderer.get(), NULL);
    }
}

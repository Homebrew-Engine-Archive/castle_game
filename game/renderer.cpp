#include "renderer.h"

#include <string>

#include <boost/current_function.hpp>
#include <boost/algorithm/clamp.hpp>

#include <game/exception.h>
#include <game/sdl_utils.h>
#include <game/collection.h>

namespace
{
    const int WindowWidth = 1024;
    const int WindowHeight = 768;

    const int WindowXPos = SDL_WINDOWPOS_UNDEFINED;
    const int WindowYPos = SDL_WINDOWPOS_UNDEFINED;

    const char *WindowTitle = "Castle game";

    const int WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    const int RendererIndex = -1;
    const int RendererFlags = SDL_RENDERER_ACCELERATED;
    
    const int MinScreenWidth = 0;
    const int MinScreenHeight = 0;
    
    // TODO sync with driver's texture max width and height
    const int MaxScreenWidth = 4096;
    const int MaxScreenHeight = 4096;

    int AdjustWidth(int width)
    {
        return boost::algorithm::clamp(width, MinScreenWidth, MaxScreenWidth);
    }

    int AdjustHeight(int height)
    {
        return boost::algorithm::clamp(height, MinScreenHeight, MaxScreenHeight);
    }
}

namespace Render
{    
    Renderer::Renderer()
        : mScreenWidth(0)
        , mScreenHeight(0)
        , mScreenFormat(SDL_PIXELFORMAT_ARGB8888)
        , mScreenClear(true)
        , mScreenTexture(nullptr)
        , mScreenSurface(nullptr)
    {
        mWindow.reset(
            SDL_CreateWindow(WindowTitle,
                             WindowXPos,
                             WindowYPos,
                             WindowWidth,
                             WindowHeight,
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

    void Renderer::CreateScreenTexture(int width, int height)
    {
        if((mScreenTexture) && (width == mScreenWidth) && (height == mScreenHeight)) {
            return;
        }
    
        mScreenTexture.reset(
            SDL_CreateTexture(
                mRenderer.get(),
                mScreenFormat,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height));

        // Inconsistent size should be reported by SDL itself
        if(!mScreenTexture) {
            throw sdl_error();
        }

        mScreenWidth = width;
        mScreenHeight = height;
    }

    void Renderer::CreateScreenSurface(int width, int height)
    {
        mScreenSurface = CreateSurface(width, height, mScreenFormat);
    }

    bool Renderer::ReallocationRequired(int width, int height)
    {
        return (width != mScreenWidth) || (height != mScreenHeight);
    }
    
    Surface Renderer::BeginFrame()
    {
        if(!mScreenTexture) {
            CreateScreenTexture(mScreenWidth, mScreenHeight);
        }

        if(!mScreenSurface) {
            CreateScreenSurface(mScreenWidth, mScreenHeight);
        }
        
        if(mScreenClear) {
            SDL_FillRect(mScreenSurface, NULL, 0);
        }
        
        return mScreenSurface;
    }

    void Renderer::EndFrame()
    {
        if(!mScreenSurface.Null()) {
            if(SDL_UpdateTexture(mScreenTexture.get(), NULL, mScreenSurface->pixels, mScreenSurface->pitch) < 0) {
                throw sdl_error();
            }

            const Rect textureRect = SurfaceBounds(mScreenSurface);
            if(SDL_RenderCopy(mRenderer.get(), mScreenTexture.get(), &textureRect, &textureRect) < 0) {
                throw sdl_error();
            }
        }

        SDL_RenderPresent(mRenderer.get());
    }

    Rect Renderer::GetScreenSize() const
    {
        Rect size;
        
        if(SDL_GetRendererOutputSize(mRenderer.get(), &size.w, &size.h) < 0) {
            throw sdl_error();
        }
        
        return size;
    }

    void Renderer::SetScreenSize(int width, int height)
    {
        AdjustScreenSize(width, height);
    }
    
    void Renderer::AdjustScreenSize(int width, int height)
    {
        mScreenSurface.reset(nullptr);
        mScreenTexture.reset(nullptr);
        
        // Cutting up and down texture height and width
        int adjustedWidth = AdjustWidth(width);
        int adjustedHeight = AdjustHeight(height);

        // NOTE
        // This is the only place we limit width and height
        if(ReallocationRequired(adjustedWidth, adjustedHeight)) {
            CreateScreenTexture(adjustedWidth, adjustedHeight);
            CreateScreenSurface(adjustedWidth, adjustedHeight);
        }
    }
    
    void Renderer::EnableClearScreen(bool on)
    {
        mScreenClear = on;
    }

    Surface Renderer::CreateImage(int width, int height)
    {
        return nullptr;
    }

    Surface Renderer::CreateImageFrom(int width, int height, int pitch, int format, char *data)
    {
        return nullptr;
    }

    void Renderer::PaintImage(const Surface &surface, const Rect &whither)
    {

    }

    sdl_error::sdl_error() throw()
        : mSDL_GetError(SDL_GetError())
    {
    }

    char const* sdl_error::what() const throw()
    {
        return mSDL_GetError.c_str();
    }
    
} // namespace Render

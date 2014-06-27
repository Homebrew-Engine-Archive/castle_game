#include "sdlrenderengine.h"

#include <stdexcept>
#include <vector>

#include <SDL2_gfxPrimitives.h>
#include <SDL.h>

#include <core/line.h>
#include <core/point.h>
#include <core/size.h>
#include <core/rect.h>
#include <core/color.h>

#include <game/image.h>
#include <game/sdl_error.h>

namespace
{
    const uint32_t WindowPixelFormat = SDL_PIXELFORMAT_RGB888;
    const int WindowWidth = 1024;
    const int WindowHeight = 768;

    const int WindowXPos = SDL_WINDOWPOS_UNDEFINED;
    const int WindowYPos = SDL_WINDOWPOS_UNDEFINED;

    const char *WindowTitle = "castle game";

    const uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;

    const int RendererIndex = -1;
    const int RendererFlags = SDL_RENDERER_ACCELERATED;

    const castle::render::OutputMode DetectOutputMode(SDL_Window &window)
    {
        int width;
        int height;
        SDL_GetWindowSize(&window, &width, &height);
        
        const uint32_t format = SDL_GetWindowPixelFormat(&window);
        return castle::render::OutputMode(width, height, format);
    }
}

namespace castle
{
    namespace render
    {
        SDLRenderEngine::~SDLRenderEngine() = default;
    
        SDLRenderEngine::SDLRenderEngine()
            : mRenderer(nullptr)
            , mWindow(nullptr)
            , mOutputMode()
            , mViewport()
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

        void SDLRenderEngine::UpdateViewport(const core::Rect &rect)
        {
            const SDL_Rect tmp {rect.X(), rect.Y(), rect.Width(), rect.Height()};
            const int code = SDL_RenderSetClipRect(mRenderer.get(), &tmp);
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
            UpdateViewport(mViewport);
            UpdateDrawColor(color);
            std::vector<SDL_Point> pts(count);
            for(size_t i = 0; i < count; ++i) {
                pts[i].x = points[i].X();
                pts[i].y = points[i].Y();
            }
            SDL_RenderDrawPoints(mRenderer.get(), pts.data(), count);
        }
    
        void SDLRenderEngine::DrawRects(const core::Rect *rects, size_t count, const core::Color &color, DrawMode mode)
        {
            UpdateViewport(mViewport);
            UpdateDrawColor(color);

            std::vector<SDL_Rect> rts(count);
            for(size_t i = 0; i < count; ++i) {
                rts[i].x = rects[i].X();
                rts[i].y = rects[i].Y();
                rts[i].w = rects[i].Width();
                rts[i].h = rects[i].Height();
            }
            switch(mode) {
            case DrawMode::Outline:
                {
                    const int code = SDL_RenderDrawRects(mRenderer.get(), rts.data(), count);
                    throw_sdl_error(code);
                }
                return;
            case DrawMode::Filled:
                {
                    const int code = SDL_RenderFillRects(mRenderer.get(), rts.data(), count);
                    throw_sdl_error(code);
                }
                return;
            default:
                throw draw_mode_error();
            }
        }
    
        void SDLRenderEngine::DrawLines(const core::Line *lines, size_t count, const core::Color &color)
        {
            UpdateViewport(mViewport);
            UpdateDrawColor(color);
        
            for(size_t i = 0; i < count; ++i) {
                const auto x1 = lines[i].p1.X();
                const auto y1 = lines[i].p1.Y();
                const auto x2 = lines[i].p2.X();
                const auto y2 = lines[i].p2.Y();
                const int code = SDL_RenderDrawLine(mRenderer.get(), x1, y1, x2, y2);
                throw_sdl_error(code);
            }
        }

        void SDLRenderEngine::DrawPolygon(const core::Point *points, size_t count, const core::Color &color, DrawMode mode)
        {
            if(count == 0) {
                return;
            }
        
            UpdateViewport(mViewport);
            UpdateDrawColor(color);
        
            const size_t polycount = count + 1;
            std::vector<Sint16> xs(polycount);
            std::vector<Sint16> ys(polycount);
        
            for(size_t i = 0; i < count; ++i) {
                xs[i] = points[i].X();
                ys[i] = points[i].Y();
            }
            xs[count] = points[0].X();
            ys[count] = points[0].Y();
        
            switch(mode) {
            case DrawMode::Filled:
                filledPolygonRGBA(mRenderer.get(), xs.data(), ys.data(), polycount, color.r, color.g, color.b, color.a);
                return;
            case DrawMode::Outline:
                polygonRGBA(mRenderer.get(), xs.data(), ys.data(), polycount, color.r, color.g, color.b, color.a);
                return;
            default:
                throw draw_mode_error();
            }
        }
    
        void SDLRenderEngine::DrawImage(const Image &image, const core::Rect &source, const core::Point &target)
        {
            const core::Rect screenRect(target.X(),
                                        target.Y(),
                                        source.Width(),
                                        source.Height());
            if(Intersects(screenRect, mViewport)) {
                UpdateViewport(mViewport);
                SDL_Surface *surface = image.GetSurface();
                TexturePtr texture(
                    SDL_CreateTextureFromSurface(mRenderer.get(), surface));
                SDL_SetTextureAlphaMod(texture.get(), mOpacityMod);
                const SDL_Rect srcrect {source.X(), source.Y(), source.Width(), source.Height()};
                const SDL_Rect dstrect {target.X(), target.Y(), source.Width(), source.Height()};
                SDL_RenderCopy(mRenderer.get(), texture.get(), &srcrect, &dstrect);
            }
        }

        void SDLRenderEngine::DrawImageTiled(const Image &image, const core::Rect &source, const core::Rect &target)
        {
        }
    
        void SDLRenderEngine::DrawImageScaled(const Image &image, const core::Rect &source, const core::Rect &target)
        {
        }
    
        void SDLRenderEngine::SetViewport(const core::Rect &rect)
        {
            mViewport = rect;
        }
    
        void SDLRenderEngine::ClearOutput(const core::Color &color)
        {
            SDL_RenderSetClipRect(mRenderer.get(), NULL);
            SDL_SetRenderDrawColor(mRenderer.get(), color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(mRenderer.get(), NULL);
        }
    }
}

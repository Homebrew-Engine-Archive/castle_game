#include "softwarerenderengine.h"

#include <cassert>

#include <vector>
#include <stdexcept>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include <game/color.h>
#include <game/point.h>
#include <game/rect.h>
#include <game/size.h>
#include <game/line.h>
#include <game/image.h>
#include <game/clamp.h>

namespace
{
    const uint32_t WindowPixelFormat = SDL_PIXELFORMAT_RGB888;
    const int WindowWidth = 1024;
    const int WindowHeight = 768;

    const int WindowXPos = SDL_WINDOWPOS_UNDEFINED;
    const int WindowYPos = SDL_WINDOWPOS_UNDEFINED;

    const char *WindowTitle = "castle game";

    const int WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    const int RendererIndex = -1;
    const int RendererFlags = SDL_RENDERER_ACCELERATED;
}

namespace castle
{
    namespace render
    {
        SoftwareRenderEngine::~SoftwareRenderEngine() = default;
        SoftwareRenderEngine::SoftwareRenderEngine()
            : mScreenRenderer(nullptr)
            , mWindow(nullptr)
            , mOutputMode(WindowWidth, WindowHeight, WindowPixelFormat)
            , mFrameOutputMode(mOutputMode)
            , mScreenImage(nullptr)
            , mScreenTexture(nullptr)
            , mPrimitiveRenderer(nullptr)
            , mOpacityMod(0)
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
        
            mScreenRenderer.reset(
                SDL_CreateRenderer(mWindow.get(),
                                   RendererIndex,
                                   RendererFlags));
            if(!mScreenRenderer) {
                throw sdl_error();
            }
        }

        void SoftwareRenderEngine::SetOpacityMod(int opacity)
        {
            mOpacityMod = opacity;
        }

        bool SoftwareRenderEngine::ReallocationRequired(const OutputMode &mode) const
        {
            if((!mScreenImage) || (!mScreenTexture) || (!mPrimitiveRenderer)) {
                return true;
            }

            return mFrameOutputMode != mOutputMode;
        }
    
        void SoftwareRenderEngine::BeginFrame()
        {
            if(ReallocationRequired(mOutputMode)) {
                mPrimitiveRenderer = nullptr;
                mScreenImage = nullptr;
                mScreenTexture = nullptr;
            }
        
            if(!mScreenTexture) {
                TexturePtr temp(
                    SDL_CreateTexture(
                        mScreenRenderer.get(),
                        mOutputMode.Format(),
                        SDL_TEXTUREACCESS_STREAMING,
                        mOutputMode.Width(),
                        mOutputMode.Height()));

                /** Incorrect size or format will be reported here **/
                if(!temp) {
                    throw sdl_error();
                }

                mScreenTexture = std::move(temp);
            }

            if(!mScreenImage) {
                const Image temp = CreateImage(
                    mOutputMode.Width(),
                    mOutputMode.Height(),
                    mOutputMode.Format());
                if(!temp) {
                    throw sdl_error();
                }

                if(SDL_SetSurfaceBlendMode(temp, SDL_BLENDMODE_NONE) < 0) {
                    throw sdl_error();
                }
        
                mScreenImage = temp;
            }

            if(!mPrimitiveRenderer) {
                RendererPtr Renderer(SDL_CreateSoftwareRenderer(mScreenImage));
                if(!Renderer) {
                    throw sdl_error();
                }

                mPrimitiveRenderer = std::move(Renderer);
            }

            mFrameOutputMode = mOutputMode;
            mViewport = core::Rect(mFrameOutputMode.Width(),
                                   mFrameOutputMode.Height());
        }
    
        void SoftwareRenderEngine::EndFrame()
        {
            assert(mScreenImage != nullptr);
            assert(mScreenTexture != nullptr);
            assert(mScreenRenderer != nullptr);
        
            const ImageLocker lock(mScreenImage);
            
            if(SDL_UpdateTexture(mScreenTexture.get(), NULL, lock.Data(), mScreenImage.RowStride()) < 0) {
                throw sdl_error();
            }

            const core::Rect textureRect(mScreenImage);
            if(SDL_RenderCopy(mScreenRenderer.get(), mScreenTexture.get(), &textureRect, &textureRect) < 0) {
                throw sdl_error();
            }
        
            SDL_RenderPresent(mScreenRenderer.get());
        }
    
        void SoftwareRenderEngine::SetOutputMode(const OutputMode &mode)
        {
            mOutputMode = mode;
        }

        const OutputMode SoftwareRenderEngine::GetOutputMode() const
        {
            return mOutputMode;
        }
        
        const core::Size SoftwareRenderEngine::GetMaxOutputSize() const
        {
            assert(mScreenRenderer != nullptr);
            SDL_RendererInfo info;
            if(SDL_GetRendererInfo(mScreenRenderer.get(), &info) < 0) {
                throw sdl_error();
            }
            return core::Size(info.max_texture_width, info.max_texture_height);
        }

        void SoftwareRenderEngine::UpdateViewport(const core::Rect &rect)
        {
            if(SDL_RenderSetClipRect(mPrimitiveRenderer.get(), &rect) < 0) {
                throw sdl_error();
            }
        }

        void SoftwareRenderEngine::UpdateDrawColor(const core::Color &color)
        {
            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            if(color.a == 0xff) {
                blendMode = SDL_BLENDMODE_NONE;
            }
            if(SDL_SetRenderDrawBlendMode(mPrimitiveRenderer.get(), blendMode) < 0) {
                throw sdl_error();
            }
        
            if(SDL_SetRenderDrawColor(mPrimitiveRenderer.get(), color.r, color.g, color.b, color.a) < 0) {
                throw sdl_error();
            }
        }
    
        void SoftwareRenderEngine::DrawPoints(const core::Point *points, size_t count, const core::Color &color)
        {
            UpdateDrawColor(color);
            UpdateViewport(mViewport);
            if(SDL_RenderDrawPoints(mPrimitiveRenderer.get(), points, count) < 0) {
                throw sdl_error();
            }
        }
    
        void SoftwareRenderEngine::DrawRects(const core::Rect *rects, size_t count, const core::Color &color, DrawMode mode)
        {
            UpdateDrawColor(color);
            UpdateViewport(mViewport);

            switch(mode) {
            case DrawMode::Outline:
                if(SDL_RenderDrawRects(mPrimitiveRenderer.get(), rects, count) < 0) {
                    throw sdl_error();
                }
                return;
            case DrawMode::Filled:
                if(SDL_RenderFillRects(mPrimitiveRenderer.get(), rects, count) < 0) {
                    throw sdl_error();
                }
                return;
            default:
                throw wrong_draw_mode();
            }
        }
    
        void SoftwareRenderEngine::DrawLines(const core::Line *lines, size_t count, const core::Color &color)
        {
            UpdateViewport(mViewport);
            UpdateDrawColor(color);
        
            for(size_t i = 0; i < count; ++i) {
                const auto x1 = lines[i].p1.x;
                const auto y1 = lines[i].p1.y;
                const auto x2 = lines[i].p2.x;
                const auto y2 = lines[i].p2.y;
                if(SDL_RenderDrawLine(mPrimitiveRenderer.get(), x1, y1, x2, y2) < 0) {
                    throw sdl_error();
                }
            }
        }
    
        void SoftwareRenderEngine::DrawPolygon(const core::Point *points, size_t count, const core::Color &color, DrawMode mode)
        {
            if(count == 0) {
                return;
            }
        
            UpdateViewport(mViewport);

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
                filledPolygonRGBA(mPrimitiveRenderer.get(), xs.data(), ys.data(), polycount, color.r, color.g, color.b, color.a);
                return;
            case DrawMode::Outline:
                polygonRGBA(mPrimitiveRenderer.get(), xs.data(), ys.data(), polycount, color.r, color.g, color.b, color.a);
                return;
            default:
                throw wrong_draw_mode();
            }
        }    

        void SoftwareRenderEngine::DrawImage(const Image &image, const core::Rect &subrect, const core::Point &targetPoint)
        {
            assert(!mScreenImage.Null());
            mScreenImage.SetClipRect(mViewport);
            SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
            if(mOpacityMod != 0xff) {
                blendMode = SDL_BLENDMODE_BLEND;
            } else if(SDL_ISPIXELFORMAT_ALPHA(ImageFormat(image).format)) {
                blendMode = SDL_BLENDMODE_BLEND;
            }
            SDL_BlendMode oldBlendMode;
            SDL_GetSurfaceBlendMode(image, &oldBlendMode);
            SDL_SetSurfaceBlendMode(image, blendMode);
            SDL_SetSurfaceAlphaMod(image, mOpacityMod);
            castle::CopyImage(image, subrect, mScreenImage, targetPoint);
            SDL_SetSurfaceBlendMode(image, oldBlendMode);
        }

        void SoftwareRenderEngine::DrawImageTiled(const Image &image, const core::Rect &source, const core::Rect &target)
        {
            DrawImage(image, source, core::TopLeft(target));
        }
    
        void SoftwareRenderEngine::DrawImageScaled(const Image &image, const core::Rect &source, const core::Rect &target)
        {
            DrawImage(image, source, core::TopLeft(target));
        }
    
        void SoftwareRenderEngine::SetViewport(const core::Rect &rect)
        {
            mViewport = rect;
        }

        void SoftwareRenderEngine::ClearOutput(const core::Color &color)
        {
            assert(mScreenImage != nullptr);
            const auto pixel = color.ConvertTo(mFrameOutputMode.Format());
            SDL_SetClipRect(mScreenImage, NULL);
            SDL_FillRect(mScreenImage, NULL, pixel);
        }
    }
}

#include "renderer.h"

#include <cassert>

#include <core/size.h>
#include <game/outputmode.h>
#include <core/rect.h>
#include <core/point.h>
#include <core/color.h>
#include <game/sdl_error.h>
#include <game/sdl_utils.h>
#include <game/fontengine.h>
#include <game/softwarerenderengine.h>
#include <game/sdlrenderengine.h>

namespace
{
    const int MaxOpacity = 0xff;

    /** dstA = srcA * opacity / MaxOpacity
        srcA = dstA * MaxOpacity / opacity
    **/
    
    constexpr int GetBlendedOpacity(int globalOpacity, int opacity)
    {
        return (globalOpacity * opacity) / MaxOpacity;
    }

    constexpr int GetUnblendedOpacity(int globalOpacity, int opacity)
    {
        return (globalOpacity * MaxOpacity) / opacity;
    }

    constexpr const core::Color BlendColor(const core::Color &color, int opacity)
    {
        return core::Color(color.r, color.g, color.b, GetBlendedOpacity(opacity, color.a));
    }
}

namespace castle
{
    namespace render
    {
        Renderer::Renderer(Renderer const&) = delete;
        Renderer& Renderer::operator=(Renderer const&) = delete;
        Renderer::Renderer(Renderer&&) = default;
        Renderer& Renderer::operator=(Renderer&&) = delete;
        Renderer::~Renderer() = default;
    
        Renderer::Renderer()
            : mRenderEngine(new SoftwareRenderEngine)
            , mFontEngine(new FontEngine)
            , mBoundPalette()
            , mBoundImage(nullptr)
            , mBoundAlphaChannel(nullptr)
            , mClipStack()
            , mOpacityStack()
            , mOpacity(MaxOpacity)
            , mBoundFont()
        {
        }

        FontEngine& Renderer::GetFontEngine()
        {
            return *mFontEngine;
        }

        RenderEngine& Renderer::GetRenderEngine()
        {
            return *mRenderEngine;
        }
        
        void Renderer::LoadFont(const core::Font &font)
        {
            mFontEngine->LoadFont(font);
        }

        void Renderer::DrawText(const std::string &text, const core::Point &target)
        {
            const core::Point screenCoords = ToScreenCoords(target);
            mFontEngine->DrawText(*mRenderEngine, screenCoords, mBoundFont, text, mDrawColor, mBackColor);
        }

        const core::Size Renderer::TextSize(const core::Font &font, const std::string &text) const
        {
            return mFontEngine->TextSize(mBoundFont, text);
        }
        
        const core::Rect Renderer::TextBoundingRect(const core::Font &font, const std::string &text) const
        {
            const core::Point screenOrigin = GetScreenOrigin();
            const core::Size textSize = mFontEngine->TextSize(font, text);
            
            return core::Rect(
                screenOrigin.X(), screenOrigin.Y(),
                textSize.Width(), textSize.Height());
        }
        
        void Renderer::BindFont(const core::Font &font)
        {
            mBoundFont = font;
        }
        
        void Renderer::BeginFrame()
        {
            const OutputMode mode = mRenderEngine->GetOutputMode();
            mClipStack.clear();
            mClipStack.push_back(
                core::Rect(mode.Width(), mode.Height()));
            
            mScreenRectStack.clear();
            mScreenRectStack.push_back(
                core::Rect(mode.Width(), mode.Height()));
        
            mRenderEngine->BeginFrame();
            mRenderEngine->ClearOutput(core::colors::Black);

            mOpacity = MaxOpacity;
        }
    
        void Renderer::EndFrame()
        {
            mRenderEngine->EndFrame();
        }

        const core::Point Renderer::GetMaxOutputSize() const
        {
            return GetOutputSize();
        }
    
        const core::Point Renderer::GetOutputSize() const
        {
            const OutputMode mode = mRenderEngine->GetOutputMode();
            return core::Point(mode.Width(), mode.Height());
        }
    
        const core::Rect Renderer::GetViewport() const
        {
            assert(!mClipStack.empty());
            return mClipStack.back();
        }
    
        const core::Rect Renderer::GetScreenRect() const
        {
            assert(!mScreenRectStack.empty());
            return core::Rect(mScreenRectStack.back().Width(), mScreenRectStack.back().Height());
        }

        const core::Size Renderer::GetScreenSize() const
        {
            assert(!mScreenRectStack.empty());
            return core::RectSize(mScreenRectStack.back());
        }

        const core::Point Renderer::GetScreenOrigin() const
        {
            assert(!mScreenRectStack.empty());
            return core::TopLeft(mScreenRectStack.back());
        }
        
        void Renderer::SetViewport(const core::Rect &clipArea)
        {
            if(core::RectEmpty(clipArea)) {
                mClipStack.push_back(core::Rect());
                mScreenRectStack.push_back(core::Rect());
                return;
            }
        
            const core::Rect screenArea = ToScreenCoords(clipArea);
            const core::Rect oldClipArea = GetViewport();
            const core::Rect newClipArea = core::Intersection(oldClipArea, screenArea);
            mClipStack.push_back(newClipArea);
            mScreenRectStack.push_back(screenArea);

            mRenderEngine->SetViewport(newClipArea);
        }

        void Renderer::RestoreViewport()
        {
            assert(!mScreenRectStack.empty());
            assert(!mClipStack.empty());
            mScreenRectStack.pop_back();
            mClipStack.pop_back();
            mRenderEngine->SetViewport(GetViewport());
        }

        void Renderer::Opacity(int opacity)
        {
            mOpacityStack.push_back(mOpacity);
            mOpacity = GetBlendedOpacity(mOpacity, opacity);
            mRenderEngine->SetOpacityMod(mOpacity);
        }

        void Renderer::RestoreOpacity()
        {
            mOpacity = mOpacityStack.back();
            mOpacityStack.pop_back();
            mRenderEngine->SetOpacityMod(mOpacity);
        }
    
        const core::Rect Renderer::ToScreenCoords(const core::Rect &relative) const
        {
            const core::Point origin = core::TopLeft(mScreenRectStack.back());
            return Translated(relative, origin.X(), origin.Y());
        }

        const core::Point Renderer::ToScreenCoords(const core::Point &relative) const
        {
            return relative + core::TopLeft(mScreenRectStack.back());
        }

        const core::Rect Renderer::ToViewportCoords(const core::Rect &relative) const
        {
            const core::Point origin = -core::TopLeft(mScreenRectStack.back());
            return Translated(relative, origin.X(), origin.Y());
        }
        
        const core::Point Renderer::ToViewportCoords(const core::Point &relative) const
        {
            return relative - core::TopLeft(GetViewport());
        }
        
        void Renderer::SetScreenWidth(int width)
        {
            OutputMode mode = mRenderEngine->GetOutputMode();
            mode.SetWidth(width);
            mRenderEngine->SetOutputMode(mode);
        }
    
        void Renderer::SetScreenHeight(int height)
        {
            OutputMode mode = mRenderEngine->GetOutputMode();
            mode.SetHeight(height);
            mRenderEngine->SetOutputMode(mode);
        }
    
        void Renderer::SetScreenFormat(int format)
        {
            OutputMode mode = mRenderEngine->GetOutputMode();
            mode.SetFormat(format);
            mRenderEngine->SetOutputMode(mode);
        }

        void Renderer::SetDrawColor(core::Color color)
        {
            mDrawColor = std::move(color);
        }
        
        void Renderer::SetBackColor(core::Color color)
        {
            mBackColor = std::move(color);
        }
        
        void Renderer::BindImage(const core::Image &surface)
        {
            mBoundImage = surface;
        }
    
        void Renderer::BindPalette(const core::Palette &palette)
        {
            mBoundPalette = palette;
        }
    
        void Renderer::BindAlphaChannel(const core::Image &surface)
        {
            mBoundAlphaChannel = surface;
        }
    
        void PaintRhombus(RenderEngine &engine, const core::Rect &bounds, const core::Color &color, DrawMode mode)
        {
            const auto x1 = bounds.X1();
            const auto y1 = bounds.Y1();
            const auto x2 = bounds.X2();
            const auto y2 = bounds.Y2();

            const auto centerX = (x1 + x2) / 2;
            const auto centerY = (y1 + y2) / 2;

            constexpr int NumPoints = 5;
            const core::Point points[NumPoints] = {
                {x1, centerY},
                {centerX, y1},
                {x2, centerY},
                {centerX, y2},
                {x1, centerY}
            };

            engine.DrawPolygon(&points[0], NumPoints, color, mode);
        }
    
        void Renderer::FillRhombus(const core::Rect &rect)
        {
            const core::Rect translatedRect = ToScreenCoords(rect);
            const core::Color blended = BlendColor(mDrawColor, mOpacity);
            PaintRhombus(*mRenderEngine, translatedRect, blended, DrawMode::Filled);
        }

        void Renderer::DrawRhombus(const core::Rect &rect)
        {
            const core::Rect translatedRect = ToScreenCoords(rect);
            const core::Color blended = BlendColor(mDrawColor, mOpacity);
            PaintRhombus(*mRenderEngine, translatedRect, blended, DrawMode::Outline);
        }

        void Renderer::FillFrame(const core::Rect &rect)
        {        
            const core::Rect unclippedRect = ToScreenCoords(rect);
            const core::Color blended = BlendColor(mDrawColor, mOpacity);
            mRenderEngine->DrawRects(&unclippedRect, 1, blended, DrawMode::Filled);
        }

        void Renderer::DrawFrame(const core::Rect &rect)
        {
            const core::Rect unclippedRect = ToScreenCoords(rect);
            const core::Color blended = BlendColor(mDrawColor, mOpacity);
            mRenderEngine->DrawRects(&unclippedRect, 1, blended, DrawMode::Outline);
        }

        void PaintPyramid(RenderEngine &engine, const core::Rect &top, const core::Rect &bottom, const core::Color &color, DrawMode mode)
        {
            PaintRhombus(engine, bottom, color, mode);
            PaintRhombus(engine, top, color, mode);

            const auto tx1 = top.X1();
            const auto ty1 = top.Y1();
            const auto tx2 = top.X2();
            const auto ty2 = top.Y2();

            const auto tcenterX = (tx1 + tx2) / 2;
            const auto tcenterY = (ty1 + ty2) / 2;

            const auto bx1 = bottom.X1();
            const auto by1 = bottom.Y1();
            const auto bx2 = bottom.X2();
            const auto by2 = bottom.Y2();

            const auto bcenterX = (bx1 + bx2) / 2;
            const auto bcenterY = (by1 + by2) / 2;

            {
                constexpr int NumPoints = 5;
                const core::Point points[NumPoints] = {
                    {tx1, tcenterY},
                    {tcenterX, ty2},
                    {bcenterX, by2},
                    {bx1, bcenterY},
                    {tx1, tcenterX}
                };
                engine.DrawPolygon(&points[0], NumPoints, color, mode);
            }

            {
                constexpr int NumPoints = 5;
                const core::Point points[NumPoints] = {
                    {tcenterX, ty2},
                    {tx2, tcenterY},
                    {bx2, bcenterY},
                    {bcenterX, by2},
                    {tcenterX, ty2}
                };
                engine.DrawPolygon(&points[0], NumPoints, color, mode);
            }
        }
    
        void Renderer::Blit(const core::Rect &source, const core::Point &target)
        {        
            mRenderEngine->SetOpacityMod(mOpacity);
        
            if(core::IsPalettized(mBoundImage)) {
                mBoundImage.AttachPalette(mBoundPalette);
            }

            const core::Point screenCoords = ToScreenCoords(target);
            mRenderEngine->DrawImage(mBoundImage, source, screenCoords);
        }
    
        void Renderer::BlitTiled(const core::Rect &source, const core::Rect &target)
        {
            Blit(source, core::TopLeft(target));
        }

        void Renderer::BlitScaled(const core::Rect &source, const core::Rect &target)
        {
            Blit(source, core::TopLeft(target));
        }
    } // namespace render
}

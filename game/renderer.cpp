#include "renderer.h"

#include <cassert>

#include <game/outputmode.h>
#include <game/rect.h>
#include <game/point.h>
#include <game/color.h>
#include <game/gm1palette.h>
#include <game/sdl_error.h>
#include <game/sdl_utils.h>
#include <game/fontmanager.h>

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

namespace Render
{
    Renderer::Renderer(Renderer const&) = delete;
    Renderer& Renderer::operator=(Renderer const&) = delete;
    Renderer::Renderer(Renderer&&) = default;
    Renderer& Renderer::operator=(Renderer&&) = delete;
    Renderer::~Renderer() = default;
    
    Renderer::Renderer(RenderEngine &renderEngine, FontManager &fontEngine)
        : mRenderEngine(renderEngine)
        , mFontManager(fontEngine)
        , mBoundPalette()
        , mBoundSurface(nullptr)
        , mBoundAlphaChannel(nullptr)
        , mClipStack()
        , mOpacityStack()
        , mOpacity(MaxOpacity)
        , mBoundFont()
    {
    }

    FontManager& Renderer::GetFontManager()
    {
        return mFontManager;
    }
    
    FontManager const& Renderer::GetFontManager() const
    {
        return mFontManager;
    }

    RenderEngine& Renderer::GetRenderEngine()
    {
        return mRenderEngine;
    }
    
    RenderEngine const& Renderer::GetRenderEngine() const
    {
        return mRenderEngine;
    }
        
    void Renderer::BeginFrame()
    {
        const OutputMode mode = mRenderEngine.GetOutputMode();
        mClipStack.clear();
        mClipStack.push_back(
            core::Rect(mode.Width(), mode.Height()));
            
        mScreenRectStack.clear();
        mScreenRectStack.push_back(
            core::Rect(mode.Width(), mode.Height()));
        
        mRenderEngine.BeginFrame();
        mRenderEngine.ClearOutput(core::colors::Black);

        mOpacity = MaxOpacity;
    }
    
    void Renderer::EndFrame()
    {
        mRenderEngine.EndFrame();
    }

    const core::Point Renderer::GetMaxOutputSize() const
    {
        return GetOutputSize();
    }
    
    const core::Point Renderer::GetOutputSize() const
    {
        const OutputMode mode = mRenderEngine.GetOutputMode();
        return core::Point(mode.Width(), mode.Height());
    }
    
    const core::Rect Renderer::GetScreenClipRect() const
    {
        assert(!mClipStack.empty());
        return mClipStack.back();
    }
    
    const core::Rect Renderer::GetScreenRect() const
    {
        assert(!mScreenRectStack.empty());
        return core::Rect(mScreenRectStack.back().w, mScreenRectStack.back().h);
    }

    void Renderer::ClipRect(const core::Rect &clipArea)
    {
        if(core::RectEmpty(clipArea)) {
            mClipStack.push_back(core::Rect());
            mScreenRectStack.push_back(core::Rect());
            return;
        }
        
        const core::Rect unwindedArea = UnwindClipRect(clipArea);
        const core::Rect oldClipArea = GetScreenClipRect();
        const core::Rect newClipArea = Intersection(oldClipArea, unwindedArea);
        mClipStack.push_back(newClipArea);
        mScreenRectStack.push_back(unwindedArea);

        mRenderEngine.ClipRect(newClipArea);
    }

    void Renderer::RestoreClipRect()
    {
        assert(!mScreenRectStack.empty());
        assert(!mClipStack.empty());
        mScreenRectStack.pop_back();
        mClipStack.pop_back();
        mRenderEngine.ClipRect(GetScreenClipRect());
    }

    void Renderer::Opacity(int opacity)
    {
        mOpacityStack.push_back(mOpacity);
        mOpacity = GetBlendedOpacity(mOpacity, opacity);
        mRenderEngine.SetOpacityMod(mOpacity);
    }

    void Renderer::RestoreOpacity()
    {
        mOpacity = mOpacityStack.back();
        mOpacityStack.pop_back();
        mRenderEngine.SetOpacityMod(mOpacity);
    }
    
    const core::Rect Renderer::UnwindClipRect(const core::Rect &rect) const
    {
        return Translated(rect, core::TopLeft(
                              (mScreenRectStack.empty()
                               ? core::Rect()
                               : mScreenRectStack.back())));
    }

    const core::Point Renderer::ClipPoint(const core::Point &point) const
    {
        return point - core::TopLeft(GetScreenClipRect());
    }
    
    void Renderer::SetScreenWidth(int width)
    {
        OutputMode mode = mRenderEngine.GetOutputMode();
        mode.SetWidth(width);
        mRenderEngine.SetOutputMode(mode);
    }
    
    void Renderer::SetScreenHeight(int height)
    {
        OutputMode mode = mRenderEngine.GetOutputMode();
        mode.SetHeight(height);
        mRenderEngine.SetOutputMode(mode);
    }
    
    void Renderer::SetScreenFormat(int format)
    {
        OutputMode mode = mRenderEngine.GetOutputMode();
        mode.SetFormat(format);
        mRenderEngine.SetOutputMode(mode);
    }
    
    void Renderer::BindSurface(const Surface &surface)
    {
        mBoundSurface = surface;
    }
    
    void Renderer::BindPalette(const GM1::Palette &palette)
    {
        mBoundPalette = palette;
    }
    
    void Renderer::BindAlphaChannel(const Surface &surface)
    {
        mBoundAlphaChannel = surface;
    }

    void Renderer::BindFont(const core::Font &font)
    {
        mBoundFont = font;
    }
    
    void PaintRhombus(RenderEngine &engine, const core::Rect &bounds, const core::Color &color, DrawMode mode)
    {
        const auto x1 = bounds.x;
        const auto y1 = bounds.y;
        const auto x2 = bounds.x + bounds.w;
        const auto y2 = bounds.y + bounds.h;

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
    
    void Renderer::FillRhombus(const core::Rect &rect, const core::Color &color)
    {
        const core::Rect translatedRect = UnwindClipRect(rect);
        const core::Color blended = BlendColor(color, mOpacity);
        PaintRhombus(mRenderEngine, translatedRect, blended, DrawMode::Filled);
    }

    void Renderer::DrawRhombus(const core::Rect &rect, const core::Color &color)
    {
        const core::Rect translatedRect = UnwindClipRect(rect);
        const core::Color blended = BlendColor(color, mOpacity);
        PaintRhombus(mRenderEngine, translatedRect, blended, DrawMode::Outline);
    }

    void Renderer::FillFrame(const core::Rect &rect, const core::Color &color)
    {        
        const core::Rect unclippedRect = UnwindClipRect(rect);
        const core::Color blended = BlendColor(color, mOpacity);
        mRenderEngine.DrawRects(&unclippedRect, 1, blended, DrawMode::Filled);
    }

    void Renderer::DrawFrame(const core::Rect &rect, const core::Color &color)
    {
        const core::Rect unclippedRect = UnwindClipRect(rect);
        const core::Color blended = BlendColor(color, mOpacity);
        mRenderEngine.DrawRects(&unclippedRect, 1, blended, DrawMode::Outline);
    }

    void PaintPyramid(RenderEngine &engine, const core::Rect &top, const core::Rect &bottom, const core::Color &color, DrawMode mode)
    {
        // PaintRhombus(engine, bottom, color, mode);
        // PaintRhombus(engine, top, color, mode);

        const auto tx1 = top.x;
        const auto ty1 = top.y;
        const auto tx2 = top.x + top.w;
        const auto ty2 = top.y + top.h;

        const auto tcenterX = (tx1 + tx2) / 2;
        const auto tcenterY = (ty1 + ty2) / 2;

        const auto bx1 = bottom.x;
        const auto by1 = bottom.y;
        const auto bx2 = bottom.x + bottom.w;
        const auto by2 = bottom.y + bottom.h;

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
    
    void Renderer::Blit(const core::Rect &textureSubRect, const core::Rect &screenSubRect)
    {        
        mRenderEngine.SetOpacityMod(mOpacity);
        
        if(SDL_ISPIXELFORMAT_INDEXED(SurfaceFormat(mBoundSurface).format)) {
            SDL_SetSurfacePalette(mBoundSurface, &mBoundPalette.asSDLPalette());
        }

        const core::Rect unclippedRect = UnwindClipRect(screenSubRect);
        mRenderEngine.DrawSurface(mBoundSurface, textureSubRect, unclippedRect);
    }
    
    void Renderer::BlitTiled(const core::Rect &from, const core::Rect &to)
    {
        
    }

    void Renderer::BlitScaled(const core::Rect &from, const core::Rect &to)
    {

    }

} // namespace Render

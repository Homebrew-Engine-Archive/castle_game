#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>

#include <SDL.h>

#include <game/font.h>
#include <game/rect.h>
#include <game/renderengine.h>
#include <game/gm1palette.h>
#include <game/surface.h>
#include <game/filesystem.h>

namespace core
{
    class Size;
    class Point;
    class Color;
}

namespace GM1
{
    class Palette;
}

namespace Render
{
    class FontManager;
}

namespace Render
{
    class Renderer
    {
    public:
        explicit Renderer(RenderEngine &renderEngine, FontManager &fontEngine);
        Renderer(Renderer const&);
        Renderer& operator=(Renderer const&);
        Renderer(Renderer&&);
        Renderer& operator=(Renderer&&);
        ~Renderer();

        FontManager& GetFontManager();
        FontManager const& GetFontManager() const;

        RenderEngine& GetRenderEngine();
        RenderEngine const& GetRenderEngine() const;
        
        void BeginFrame();
        void EndFrame();

        const core::Point GetMaxOutputSize() const;
        const core::Point GetOutputSize() const;
        void SetScreenWidth(int width);
        void SetScreenHeight(int height);
        void SetScreenFormat(int format);

        void Opacity(int opacity);
        void RestoreOpacity();

        void ClipRect(const core::Rect &subrect);
        void RestoreClipRect();
        const core::Rect GetScreenClipRect() const;
        const core::Rect GetScreenRect() const;

        const core::Rect UnwindClipRect(const core::Rect &relative) const;
        const core::Point ClipPoint(const core::Point &point) const;

        const core::Size TextSize(const core::Font &font, const std::string &text) const;
        const core::Rect TextBoundingRect(const core::Font &font, const std::string &text) const;

        void BindFont(const core::Font &font);
        void DrawText(const std::string &text, const core::Color &color);
        
        const core::Color GetContextAlpha(const core::Color &color) const;
        
        void BindSurface(const Surface &surface);
        void BindPalette(const GM1::Palette &palette);
        void BindAlphaChannel(const Surface &surface);

        void Blit(const core::Rect &textureSubRect, const core::Rect &screenSubRect);
        void BlitTiled(const core::Rect &textureSubRect, const core::Rect &screenSubRect);
        void BlitScaled(const core::Rect &textureSubRect, const core::Rect &screenSubRect);

        void DrawRhombus(const core::Rect &bounds, const core::Color &fg);
        void FillRhombus(const core::Rect &bounds, const core::Color &bg);

        void DrawFrame(const core::Rect &bounds, const core::Color &fg);
        void FillFrame(const core::Rect &bounds, const core::Color &bg);
        
    protected:
        RenderEngine &mRenderEngine;
        FontManager &mFontManager;
        GM1::Palette mBoundPalette;
        Surface mBoundSurface;
        Surface mBoundAlphaChannel;
        std::vector<core::Rect> mClipStack;
        std::vector<core::Rect> mScreenRectStack;
        std::vector<float> mOpacityStack;
        int mOpacity;
        core::Font mBoundFont;

    };
    
} // namespace Render

#endif

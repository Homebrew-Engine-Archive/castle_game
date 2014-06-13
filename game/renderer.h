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

class Color;
class Point;

namespace core
{
    class Size;
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
        ~Renderer();

        FontManager& GetFontManager();
        FontManager const& GetFontManager() const;

        RenderEngine& GetRenderEngine();
        RenderEngine const& GetRenderEngine() const;
        
        void BeginFrame();
        void EndFrame();

        const Point GetMaxOutputSize() const;
        const Point GetOutputSize() const;
        void SetScreenWidth(int width);
        void SetScreenHeight(int height);
        void SetScreenFormat(int format);

        void Opacity(int opacity);
        void RestoreOpacity();

        void ClipRect(const Rect &subrect);
        void RestoreClipRect();
        const Rect GetScreenClipRect() const;
        const Rect GetScreenRect() const;

        const Rect UnwindClipRect(const Rect &relative) const;
        const Point ClipPoint(const Point &point) const;

        const core::Size TextSize(const core::Font &font, const std::string &text) const;
        const Rect TextBoundingRect(const core::Font &font, const std::string &text) const;

        void BindFont(const core::Font &font);
        void DrawText(const std::string &text, const Color &color);
        
        const Color GetContextAlpha(const Color &color) const;
        
        void BindSurface(const Surface &surface);
        void BindPalette(const GM1::Palette &palette);
        void BindAlphaChannel(const Surface &surface);

        void Blit(const Rect &textureSubRect, const Rect &screenSubRect);
        void BlitTiled(const Rect &textureSubRect, const Rect &screenSubRect);
        void BlitScaled(const Rect &textureSubRect, const Rect &screenSubRect);

        void DrawRhombus(const Rect &bounds, const Color &fg);
        void FillRhombus(const Rect &bounds, const Color &bg);

        void DrawFrame(const Rect &bounds, const Color &fg);
        void FillFrame(const Rect &bounds, const Color &bg);
        
    protected:
        RenderEngine &mRenderEngine;
        FontManager &mFontManager;
        GM1::Palette mBoundPalette;
        Surface mBoundSurface;
        Surface mBoundAlphaChannel;
        std::vector<Rect> mClipStack;
        std::vector<Rect> mScreenRectStack;
        std::vector<float> mOpacityStack;
        int mOpacity;
        core::Font mBoundFont;

    };
    
} // namespace Render

#endif

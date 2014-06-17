#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>

#include <game/font.h>
#include <game/rect.h>
#include <game/renderengine.h>
#include <game/gm1palette.h>
#include <game/image.h>
#include <game/filesystem.h>
#include <game/color.h>

namespace core
{
    class Size;
    class Point;
}

namespace GM1
{
    class Palette;
}

namespace Castle
{
    namespace Render
    {
        class FontEngine;
    }

    namespace Render
    {
        class Renderer
        {
        public:
            explicit Renderer();
            Renderer(Renderer const&);
            Renderer& operator=(Renderer const&);
            Renderer(Renderer&&);
            Renderer& operator=(Renderer&&);
            virtual ~Renderer();

            FontEngine const& GetFontEngine() const;
            
            void BeginFrame();
            void EndFrame();

            const core::Point GetMaxOutputSize() const;
            const core::Point GetOutputSize() const;
            void SetScreenWidth(int width);
            void SetScreenHeight(int height);
            void SetScreenFormat(int format);

            void Opacity(int opacity);
            void RestoreOpacity();

            void SetViewport(const core::Rect &subrect);
            const core::Rect GetViewport() const;
            void RestoreViewport();

            const core::Size GetScreenSize() const;
            const core::Point GetScreenOrigin() const;
            const core::Rect GetScreenRect() const;

            const core::Rect ToScreenCoords(const core::Rect &relative) const;
            const core::Point ToScreenCoords(const core::Point &relative) const;
            
            const core::Rect ToViewportCoords(const core::Rect &relative) const;
            const core::Point ToViewportCoords(const core::Point &relative) const;

            const core::Size TextSize(const core::Font &font, const std::string &text) const;
            const core::Rect TextBoundingRect(const core::Font &font, const std::string &text) const;

            void SetDrawColor(core::Color color);
            void SetBackColor(core::Color color);
            
            void LoadFont(const core::Font &font);
            void BindFont(const core::Font &font);
            void DrawText(const std::string &text);
        
            void BindImage(const Image &surface);
            void BindPalette(const GM1::Palette &palette);
            void BindAlphaChannel(const Image &surface);

            void Blit(const core::Rect &source, const core::Point &target);
            void BlitTiled(const core::Rect &source, const core::Rect &target);
            void BlitScaled(const core::Rect &source, const core::Rect &target);

            void DrawRhombus(const core::Rect &target);
            void FillRhombus(const core::Rect &target);

            void DrawFrame(const core::Rect &target);
            void FillFrame(const core::Rect &target);
        
        protected:
            std::unique_ptr<RenderEngine> mRenderEngine;
            std::unique_ptr<FontEngine> mFontEngine;
            GM1::Palette mBoundPalette;
            Image mBoundImage;
            Image mBoundAlphaChannel;
            std::vector<core::Rect> mClipStack;
            std::vector<core::Rect> mScreenRectStack;
            std::vector<float> mOpacityStack;
            int mOpacity;
            core::Font mBoundFont;
            core::Color mDrawColor;
            core::Color mBackColor;

        };
    
    } // namespace Render
}

#endif

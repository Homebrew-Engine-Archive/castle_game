#ifndef RENDERER_H_
#define RENDERER_H_

#include <memory>
#include <vector>

#include <SDL.h>

#include <game/gm1palette.h>
#include <game/color.h>
#include <game/surface.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>

class Rect;
class Point;

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
        RendererPtr mRenderer;
        int mScreenWidth;
        int mScreenHeight;
        int mScreenFormat;
        TexturePtr mScreenTexture;
        Surface mScreenSurface;
        WindowPtr mWindow;
        GM1::Palette mBoundPalette;
        Surface mBoundSurface;
        Surface mBoundAlphaMap;
        std::vector<Rect> mClipStack;
        std::vector<Rect> mScreenRectStack;
        std::vector<float> mAlphaStack;
        int mAlpha;
        RendererPtr mPrimitiveRenderer;
        std::unique_ptr<FontManager> mFontManager;

        bool ReallocationRequired(int width, int heigth, int format) const;
        void CreateScreenTexture(int width, int height, int format);
        void CreateScreenSurface(int width, int height);
        void CreatePrimitiveRenderer();
        
    public:
        Renderer();
        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        ~Renderer();

        FontManager& GetFontManager();
        FontManager const& GetFontManager() const;
        
        void BeginFrame();
        void EndFrame();

        const Point GetMaxOutputSize() const;
        const Point GetOutputSize() const;
        void SetScreenSize(int width, int height);
        void SetScreenFormat(int format);
        void SetScreenMode(int width, int height, int format);

        void Alpha(int alpha);
        void Unalpha();

        void Clip(const Rect &subrect);
        void Unclip();
        const Rect GetScreenClipRect() const;
        const Rect GetScreenRect() const;
        const Rect UnclipRect(const Rect &relative) const;
        const Point ClipPoint(const Point &point) const;
        
        void BindSurface(const Surface &surface);
        void BindPalette(const GM1::Palette &palette);
        void BindAlphaMap(const Surface &surface);

        void UnbindPalette();
        void UnbindSurface();
        void UnbindAlphaMap();

        void Blit(const Rect &textureSubRect, const Rect &screenSubRect);
        void BlitTiled(const Rect &textureSubRect, const Rect &screenSubRect);
        void BlitScaled(const Rect &textureSubRect, const Rect &screenSubRect);

        void DrawRhombus(const Rect &bounds, const Color &fg);
        void FillRhombus(const Rect &bounds, const Color &bg);

        void DrawFrame(const Rect &bounds, const Color &fg);
        void FillFrame(const Rect &bounds, const Color &bg);
    };
    
} // namespace Render

#endif

#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>

#include <SDL.h>

#include <game/color.h>
#include <game/gm1palette.h>
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
    class TextRenderer;
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
        const GM1::Palette mEmptyPalette;
        GM1::Palette mBoundPalette;
        Surface mBoundTexture;
        Surface mBoundAlphaMap;
        const int mDefaultAlphaMod;
        int mAlphaMod;
        
        bool ReallocationRequired(int width, int heigth, int format);
        void CreateScreenTexture(int width, int height, int format);
        void CreateScreenSurface(int width, int height);

    public:
        Renderer();
        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        
        void BeginFrame();
        void EndFrame();

        Surface GetScreenSurface();
        TextRenderer GetTextRenderer();
        
        const Point GetOutputSize() const;
        const Rect GetScreenRect() const;
        void SetScreenSize(int width, int height);
        void SetScreenFormat(int format);
        void SetScreenMode(int width, int height, int format);

        void BindTexture(const Surface &surface);
        void BindPalette(const GM1::Palette &palette);
        void BindAlphaMap(const Surface &surface);

        void SetAlphaMod(int alpha);
        void UnsetAlphaMod();
        
        void UnbindPalette();
        void UnbindTexture();
        void UnbindAlphaMap();
        
        void BlitTexture(const Rect &textureSubRect, const Rect &screenSubRect);

        void DrawRhombus(const Rect &bounds, const Color &fg);
        void FillRhombus(const Rect &bounds, const Color &bg);
        
        void DrawFrame(const Rect &bounds, const Color &fg);
        void FillFrame(const Rect &bounds, const Color &bg);
    };
    
} // namespace Render

#endif

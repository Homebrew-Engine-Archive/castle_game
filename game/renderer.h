#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>

#include <SDL.h>

#include <game/rect.h>
#include <game/surface.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>

namespace GM1
{
    class Palette;
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
        
        bool ReallocationRequired(int width, int heigth, int format);
        void CreateScreenTexture(int width, int height, int format);
        void CreateScreenSurface(int width, int height);

    public:
        Renderer();
        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        
        const Surface BeginFrame();
        void EndFrame();
        
        const Rect GetScreenSize() const;
        void SetScreenSize(int width, int height);
        void SetScreenFormat(int format);
        void SetScreenMode(int width, int height, int format);
        
        typedef PalettePtr Palette;
        const Palette CreatePalette(const std::vector<SDL_Color> &colors);
        
        const Surface CreateImage(int width, int height, int format);
        const Surface CreateImageFrom(int width, int height, int pitch, int format, char *data);

        void PaintImage(const Surface &surface, const Rect &whither);
        
        void BindTexture(const Surface &surface);
        void BindPalette(const GM1::Palette &palette);
        void BindAlphaMap(const Surface &surface);
        
        void BlitTexture(const Rect &textureSubRect, const Rect &screenSubRect);
    };
    
} // namespace Render

#endif

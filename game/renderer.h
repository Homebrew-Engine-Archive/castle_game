#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL.h>

#include <game/rect.h>
#include <game/surface.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>

class Surface;

namespace Render
{
    struct sdl_error : public std::exception
    {
        std::string mSDL_GetError;
        sdl_error() throw();
        char const* what() const throw();
    };
    
    class Renderer
    {
        RendererPtr mRenderer;
        int mScreenWidth;
        int mScreenHeight;
        int mScreenFormat;
        bool mScreenClear;
        TexturePtr mScreenTexture;
        Surface mScreenSurface;
        WindowPtr mWindow;

        bool ReallocationRequired(int width, int heigth);
        void CreateScreenTexture(int width, int height);
        void CreateScreenSurface(int width, int height);
        
        void AdjustScreenSize(int width, int height);

    public:
        Renderer();
        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        
        Surface BeginFrame();
        void EndFrame();
        Rect GetScreenSize() const;
        void SetScreenSize(int width, int height);
        
        void EnableClearScreen(bool on);

        Surface CreateImage(int width, int height);
        Surface CreateImageFrom(int width, int height, int pitch, int format, char *data);

        void PaintImage(const Surface &surface, const Rect &whither);
    };
    
} // namespace Render

#endif

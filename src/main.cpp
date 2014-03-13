#include "main.h"

using namespace std;

int main()
{
    SDLInit(SDL_INIT_EVERYTHING);

    EnumRenderDrivers();

    const int screenwidth = 1024;
    const int screenheight = 768;
        
    WindowPtr sdl_window = WindowPtr(
        SDL_CreateWindow(
            "Stockade",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screenwidth, screenheight,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    ThrowSDLError(sdl_window);

    RendererPtr sdl_renderer = RendererPtr(
        SDL_CreateRenderer(sdl_window.get(), -1, 0));
    ThrowSDLError(sdl_renderer);
        
    Renderer renderer(sdl_renderer.get());
    PrintRendererInfo(sdl_renderer.get());
    
    RootScreen screen(&renderer);
    return screen.Exec();
}

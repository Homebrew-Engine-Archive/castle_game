#include "main.h"

using namespace std;

int main()
{
    SDLInit(SDL_INIT_EVERYTHING);

    EnumRenderDrivers();

    const int screenwidth = 1024;
    const int screenheight = 768;
    Window window("openhold", screenwidth, screenheight);
    Renderer renderer(window);

    PrintRendererInfo(renderer.GetRenderer());
    
    RootScreen screen(renderer);
    return screen.Exec();
}

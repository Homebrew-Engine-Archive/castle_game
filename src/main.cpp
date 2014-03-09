#include "main.h"

using namespace std;

int main()
{
    try {
        SDLInit(SDL_INIT_EVERYTHING);

        EnumRenderDrivers();

        const int screenwidth = 1024;
        const int screenheight = 768;
        Window window("openhold", screenwidth, screenheight);
        Renderer renderer(&window);

        PrintRendererInfo(renderer.GetRenderer());
    
        RootScreen screen(&renderer);
        return screen.Exec();
    } catch(const std::exception &e) {
        std::cerr << "Exception in main(): "
                  << e.what()
                  << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception in main()"
                  << std::endl;
    }

    return -1;
}

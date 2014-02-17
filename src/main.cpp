#include "main.h"

using namespace std;

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);    
    SDLWindow window("SH", 1024, 768);
    Renderer renderer(window);

    std::unique_ptr<Screen> rootScreen(
        new LoadingScreen(renderer));

    bool quit = false;
    
    Uint32 lastTime = 0;
    const Uint32 frameRate = 16;
    const Uint32 frameInterval = 1000 / frameRate;
    
    while(!quit && rootScreen) {
        const Uint32 currentTime = SDL_GetTicks();
        
        if(lastTime + frameInterval > currentTime) {
            SDL_Event e;
            rootScreen->OnEnterEventLoop();
            while(SDL_PollEvent(&e)) {
                switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    default:
                        rootScreen->OnEvent(e);
                        break;
                    }
                default:
                    rootScreen->OnEvent(e);
                    break;
                }
            }
        } else {
            lastTime = currentTime;
            rootScreen->OnFrame(renderer);
            renderer.Present();
        }

        if(rootScreen->Closed()) {
            rootScreen = std::move(rootScreen->NextScreen());
        }
    }
    
    return 0;
}


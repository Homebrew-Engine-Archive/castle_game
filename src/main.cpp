#include "main.h"

using namespace std;

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);    
    SDLWindow window("SH", 1024, 768);
    Renderer renderer(window);

    RunLoadingScreen(renderer);
    
    Screen *rootScreen = new GameScreen;

    bool frameLimit = true;
    const Uint32 frameRate = 16;
    const Uint32 frameInterval = 1000 / frameRate;

    bool quit = false;
    
    while(quit) {

        const Uint32 frameStart = SDL_GetTicks();
        
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            default:
                if(!rootScreen->HandleEvent(e))
                    quit = true;
                break;
            }
        }

        rootScreen->Draw(renderer);
        
        const Uint32 frameEnd = SDL_GetTicks();
        if(frameLimit && (frameEnd - frameStart) < frameInterval) {
            SDL_Delay((frameEnd - frameStart));
        }
    }
    
    return 0;
}


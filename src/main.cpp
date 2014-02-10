#include "main.h"

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);
    SDLWindow window("SH", 640, 480);
    SDLRenderer renderer(window);
    
    SDL_RWops *src = SDL_RWFromFile("/opt/stronghold/gm/tile_land3.gm1", "rb");
    
    GM1CollectionScheme scheme(src);
    DebugPrint_GM1Header(scheme.header);
    
    std::vector<TileObject> frames;
    ReadTileSet(src, scheme, frames);
    
    bool quit = false;
    while(!quit) {
        SDL_Event e;
        SDL_PumpEvents();
        if(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
                break;
            }
        }
        
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                SDL_PushEvent(&e);
                break;
            }
        }

        renderer.Present();
        SDL_Delay(100);
    }
    
    return 0;
}

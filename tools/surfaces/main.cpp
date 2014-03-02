#include <iostream>
#include "surface.h"
#include "tgx.h"
#include <SDL2/SDL.h>

using namespace std;

int main(int argc, char *argv[])
{
    try {
        SDL_Init(SDL_INIT_EVERYTHING);

        SDL_RWops *src = SDL_RWFromFile(argv[1], "rb");
        if(src == NULL)
            throw "Can't read file";
    
        Surface tgx = tgx::LoadStandaloneImage(src);
        std::clog << std::hex << tgx << std::endl;
        
        Surface ggg = tgx;
        std::clog << std::hex << ggg << std::endl;

        std::clog << std::boolalpha << tgx.Null() << ' ' << ggg.Null() << std::endl;

        BlitSurface(tgx, NULL, ggg, NULL);

        SDL_RWclose(src);
        SDL_Quit();

    } catch(const char * string) {
        std::cout << string << std::endl;
    }
    
    return 0;
}

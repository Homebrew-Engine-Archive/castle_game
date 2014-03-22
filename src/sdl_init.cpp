#include "sdl_init.h"

#include <stdexcept>
#include <sstream>
#include <SDL.h>

SDLInit::SDLInit(int flags)
{
    SDL_SetMainReady();
    if(SDL_Init(flags) < 0) {
        std::stringstream oss;
        oss << "SDL_Init failed: " << SDL_GetError();
        throw std::runtime_error(oss.str());
    }
}

SDLInit::~SDLInit()
{
    SDL_Quit();
}


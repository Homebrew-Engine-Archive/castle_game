#include "sdl_utils.h"

#include <stdexcept>

SDLInitializer::SDLInitializer(int flags)
{
    SDL_SetMainReady();
    if(SDL_Init(flags) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDLInitializer::~SDLInitializer()
{
    SDL_Quit();
}

#include "sdl_utils.h"

#include <stdexcept>

SDLInitializer::SDLInitializer()
{
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDLInitializer::~SDLInitializer()
{
    SDL_Quit();
}

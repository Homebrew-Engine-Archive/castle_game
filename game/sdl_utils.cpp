#include "sdl_utils.h"

#include <stdexcept>

SDLInitializer::SDLInitializer(int flags) throw(sdl_error)
{
    SDL_SetMainReady();
    if(SDL_Init(flags) < 0) {
        throw sdl_error();
    }
}

SDLInitializer::~SDLInitializer() throw()
{
    SDL_Quit();
}


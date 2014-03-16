#include "sdl_init.h"
#include "SDL.h"

SDLInit::SDLInit(int flags)
    throw(std::runtime_error)
{
    if(SDL_Init(flags))
        throw std::runtime_error(SDL_GetError());
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

SDLInit::~SDLInit() throw()
{
    SDL_Quit();
}


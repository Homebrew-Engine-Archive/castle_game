#include "SDLContext.h"

SDLContext::SDLContext(int flags)
    throw(std::runtime_error)
{
    if(SDL_Init(flags))
        throw std::runtime_error(SDL_GetError());
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

SDLContext::~SDLContext() throw()
{
    SDL_Quit();
}


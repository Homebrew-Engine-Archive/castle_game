#include "SDLContext.h"

SDLContext::SDLContext(int flags) throw(SDLError)
{
    if(SDL_Init(flags))
        throw SDLError(SDL_GetError());
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

SDLContext::~SDLContext() throw()
{
    SDL_Quit();
}


#include "SDLContext.h"

SDLContext::SDLContext(int flags) throw(SDLError)
{
    if(0 != SDL_Init(flags))
        throw SDLError("SDL_Init");
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

SDLContext::~SDLContext() throw()
{
    SDL_Quit();
}


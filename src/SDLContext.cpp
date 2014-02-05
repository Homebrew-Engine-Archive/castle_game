#include "SDLContext.h"

SDLContext::SDLContext(int flags) throw(SDLError)
{
    if(0 != SDL_Init(flags))
        throw SDLError("SDL_Init");
}

SDLContext::~SDLContext() throw()
{
    SDL_Quit();
}


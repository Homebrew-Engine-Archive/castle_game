#ifndef SDLCONTEXT_H_
#define SDLCONTEXT_H_

#include <SDL2/SDL.h>

#include <exception>
#include "Exceptions.h"

class SDLContext
{
public:
    SDLContext(int flags) throw(SDLError);
    ~SDLContext() throw();
};

#endif

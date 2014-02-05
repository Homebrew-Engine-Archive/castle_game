#ifndef SDLCONTEXT_H_
#define SDLCONTEXT_H_

#include <SDL2/SDL.h>

#include "errors.h"

class SDLContext
{
public:
    SDLContext(int flags) throw(SDLError);
    ~SDLContext() throw();
};

#endif

#ifndef SDLCONTEXT_H_
#define SDLCONTEXT_H_

#include <stdexcept>
#include <SDL2/SDL.h>

class SDLContext
{
public:
    SDLContext(int flags) throw(std::runtime_error);
    ~SDLContext() throw();
};

#endif

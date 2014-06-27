#ifndef SDL_INIT_H_
#define SDL_INIT_H_

#include <SDL.h>

#include <stdexcept>

#include <game/sdl_error.h>

struct SDLInitializer
{
    SDLInitializer(int flags) throw(sdl_error);
    ~SDLInitializer() throw();
};

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


#endif // SDL_INIT_H_

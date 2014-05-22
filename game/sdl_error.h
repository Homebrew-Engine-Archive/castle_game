#ifndef SDL_ERROR_H_
#define SDL_ERROR_H_

#include <SDL.h>
#include <exception>

struct sdl_error : public std::exception
{
    const char *mSDL_GetError;
    sdl_error() throw()
        : mSDL_GetError(SDL_GetError())
        { }

    char const* what() const throw() {
        return mSDL_GetError;
    }
};

#endif // SDL_ERROR_H_
#ifndef SDL_ERROR_H_
#define SDL_ERROR_H_

#include <SDL.h>
#include <string>
#include <exception>

class sdl_error : public std::exception
{
    const std::string mSDL_GetError;
    
public:
    sdl_error() throw()
        : mSDL_GetError(SDL_GetError())
        { }

    char const* what() const throw()
    {
        return mSDL_GetError.c_str();
    }
};

#endif // SDL_ERROR_H_

#ifndef SDL_ERROR_H_
#define SDL_ERROR_H_

#include <SDL.h>
#include <string>
#include <exception>

class sdl_error : public std::exception
{
public:
    sdl_error() throw()
        { }

    char const* what() const throw()
    {
        return SDL_GetError();
    }
};

namespace
{
    void throw_sdl_error(int code)
    {
        if(code < 0) {
            throw sdl_error();
        }
    }
}

#endif // SDL_ERROR_H_

#ifndef SDL_ERROR_H_
#define SDL_ERROR_H_

#include <SDL.h>
#include <string>
#include <exception>

class sdl_error : public virtual std::exception
{
    std::string mErrMsg;
public:
    sdl_error() throw()
    : mErrMsg(SDL_GetError())
        { }

    char const* what() const throw()
    {
        return mErrMsg.c_str();
    }
};

namespace
{
    inline void throw_sdl_error(int code)
    {
        if(code < 0) {
            throw sdl_error();
        }
    }
}

#endif // SDL_ERROR_H_

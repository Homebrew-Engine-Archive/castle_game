#include "sdl_utils.h"

void ThrowSDLError(const SDL_Surface *surface)
{
    if(surface == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
}

void ThrowSDLError(int code)
{
    if(code < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

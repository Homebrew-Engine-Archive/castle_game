#include "gameexception.h"

#include <SDL.h>

GameException SDLException(const std::string &where, const std::string &file, int line)
{
    GameException error;
    error.reason = SDL_GetError();
    error.where = where;
    error.file = file;
    error.line = line;
    return error;
}

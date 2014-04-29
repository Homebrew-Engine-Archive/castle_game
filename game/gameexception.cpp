#include "gameexception.h"

#include <sstream>
#include <iostream>
#include <SDL.h>

GameException::GameException(const std::string &what, const std::string &where, const std::string &file, int line)
    : mReason(what)
    , mWhere(where)
    , mFile(file)
    , mLine(line)
{
}

char const* GameException::what() const throw()
{
    return mReason.c_str();
}

GameException SDLException(const std::string &where, const std::string &file, int line)
{
    return GameException(SDL_GetError(), where, file, line);
}

std::ostream& operator<<(std::ostream &out, const GameException &error)
{
    out << "GameException:" << std::endl
        << "\tWhat = " << error.mReason << std::endl
        << "\tWhere = " << error.mWhere << std::endl
        << "\tFile = " << error.mFile << std::endl
        << "\tLine = " << error.mLine << std::endl;
    return out;
}

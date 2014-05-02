#include "exception.h"

#include <sstream>
#include <iostream>
#include <SDL.h>

namespace Castle
{
    Exception::Exception(const std::string &what, const std::string &where, const std::string &file, int line)
        : mReason(what)
        , mWhere(where)
        , mFile(file)
        , mLine(line)
    {
    }

    char const* Exception::what() const throw()
    {
        return mReason.c_str();
    }

    Exception SDLException(const std::string &where, const std::string &file, int line)
    {
        return Exception(SDL_GetError(), where, file, line);
    }
}

std::ostream& operator<<(std::ostream &out, const Castle::Exception &error)
{
    out << "GameException:" << std::endl
        << "\tWhat = " << error.mReason << std::endl
        << "\tWhere = " << error.mWhere << std::endl
        << "\tFile = " << error.mFile << std::endl
        << "\tLine = " << error.mLine << std::endl;
    return out;
}

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

    Error& Error::operator()(const std::string &attribute, const std::string &value)
    {
        mAttributes[attribute] = value;
        return *this;
    }

    const char* Error::what() const throw()
    {
        return "Error wat?!";
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

std::ostream& operator<<(std::ostream &out, const Castle::Error &error)
{
    out << "Exception: " << std::endl;
    for(auto attrib : error.mAttributes) {
        out << attrib.first << " = " << attrib.second << std::endl;
    }
    return out;
}

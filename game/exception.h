#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <iosfwd>
#include <string>
#include <exception>

namespace Castle
{
    struct Exception : public std::exception
    {
        std::string mReason;
        std::string mWhere;
        std::string mFile;
        int mLine;

        char const* what() const throw();

        Exception(const std::string &what, const std::string &where, const std::string &file, int line);
    };

    Exception SDLException(const std::string &where, const std::string &file, int line);
}

std::ostream& operator<<(std::ostream &out, const Castle::Exception &error);

#endif // EXCEPTION_H_

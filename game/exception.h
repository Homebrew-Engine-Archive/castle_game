#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <map>
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

    struct Error : public std::exception
    {
        std::map<std::string, std::string> mAttributes;

        Error& operator()(const std::string &attribute, const std::string &value);

        char const* what() const throw();
    };
}

std::ostream& operator<<(std::ostream &out, const Castle::Exception &error);
std::ostream& operator<<(std::ostream &out, const Castle::Error &error);

#endif // EXCEPTION_H_

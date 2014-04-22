#ifndef GAMEEXCEPTION_H_
#define GAMEEXCEPTION_H_

#include <string>
#include <exception>

struct GameException : public std::exception
{
    std::string reason;
    std::string where;
    std::string file;
    int line;

    char const* what() const throw() {
        return reason.c_str();
    }
};

GameException SDLException(const std::string &where, const std::string &file, int line);

#endif // GAMEEXCEPTION_H_

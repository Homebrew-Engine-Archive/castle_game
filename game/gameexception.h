#ifndef GAMEEXCEPTION_H_
#define GAMEEXCEPTION_H_

#include <iosfwd>
#include <string>
#include <exception>

struct GameException : public std::exception
{
    std::string mReason;
    std::string mWhere;
    std::string mFile;
    int mLine;

    char const* what() const throw();

    GameException(const std::string &what, const std::string &where, const std::string &file, int line);
};

std::ostream& operator<<(std::ostream &out, const GameException &error);

GameException SDLException(const std::string &where, const std::string &file, int line);

#endif // GAMEEXCEPTION_H_

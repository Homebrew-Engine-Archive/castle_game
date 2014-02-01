#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <sstream>
#include <exception>

#include <SDL2/SDL.h>

class ReadTGXError : public std::exception
{
private:
    std::string m_reason;
public:
    ReadTGXError();
    ReadTGXError(const char *reason);
    virtual const char *what() const throw();
};

class SDLError : public std::exception
{
private:
    std::string m_where;
    std::string m_message;
public:
    SDLError();
    SDLError(const char * _where);
    virtual const char * what() const throw();
    virtual const char * where() const;
};

#endif

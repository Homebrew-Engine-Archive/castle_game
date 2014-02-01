#include "Exceptions.h"

SDLError::SDLError()
    : m_message(SDL_GetError())
    , m_where()
{
}

SDLError::SDLError(const char *_where)
    : m_message(SDL_GetError())
    , m_where(_where)
{
}

const char * SDLError::what() const throw()
{
    return m_message.c_str();
}

const char * SDLError::where() const
{
    return m_where.c_str();
}

ReadTGXError::ReadTGXError()
    : m_reason("unknown")
{
}

ReadTGXError::ReadTGXError(const char *reason)
    : m_reason(reason)
{
}

const char * ReadTGXError::what() const throw()
{
    return m_reason.c_str();
}

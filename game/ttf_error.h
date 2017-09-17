#ifndef TTF_ERROR_H_
#define TTF_ERROR_H_

#include <SDL_ttf.h>
#include <exception>

class ttf_error : public std::exception
{
public:
    ttf_error() throw() {}
    
    char const* what() const throw()
    {
        return TTF_GetError();
    }
};

#endif // TTF_ERROR_H_

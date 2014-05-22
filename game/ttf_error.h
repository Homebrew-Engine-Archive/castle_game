#ifndef TTF_ERROR_H_
#define TTF_ERROR_H_

#include <SDL_ttf.h>
#include <exception>

class ttf_error : public std::exception
{
    const char *mTTF_GetError;
public:
    ttf_error() throw(): mTTF_GetError(TTF_GetError()) {}

    char const* what() const throw() {
        return mTTF_GetError;
    }
};

#endif // TTF_ERROR_H_

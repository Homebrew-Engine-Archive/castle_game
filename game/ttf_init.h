#ifndef TTF_INIT_H_
#define TTF_INIT_H_

#include <SDL_ttf.h>

#include <game/ttf_error.h>

struct TTFInitializer
{
    TTFInitializer() throw(ttf_error)
    {    
        if(TTF_Init() == -1) {
            throw ttf_error();
        }
    }

    ~TTFInitializer() throw()
    {
        TTF_Quit();
    }
};

#endif // TTF_INIT_H_

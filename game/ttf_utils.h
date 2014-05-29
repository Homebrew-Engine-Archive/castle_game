#ifndef TTF_UTILS_H_
#define TTF_UTILS_H_

#include <memory>
#include <SDL_ttf.h>
#include <game/ttf_error.h>

struct FontCloseDeleter
{
    void operator()(TTF_Font *font)
        {
            TTF_CloseFont(font);
        }
};

typedef std::unique_ptr<TTF_Font, FontCloseDeleter> FontPtr;

struct TTFInitializer final
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

#endif // TTF_UTILS_H_

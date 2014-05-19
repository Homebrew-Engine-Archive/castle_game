#ifndef SDL_UTILS_H_
#define SDL_UTILS_H_

#include <iosfwd>
#include <memory>

#include <SDL_ttf.h>
#include <SDL.h>

struct sdl_error : public std::exception
{
    const char *mSDL_GetError;
    sdl_error() throw()
        : mSDL_GetError(SDL_GetError())
        { }

    char const* what() const throw() {
        return mSDL_GetError;
    }
};

struct SDLInitializer final
{
    SDLInitializer(int flags);
    ~SDLInitializer();
};

template<class T, void (*D)(T*)>
struct SDLDeleter
{
    void operator()(T *t) const {
        D(t);
    }
};

typedef std::unique_ptr<SDL_Renderer, SDLDeleter<SDL_Renderer, SDL_DestroyRenderer>> RendererPtr;
typedef std::unique_ptr<SDL_Texture, SDLDeleter<SDL_Texture, SDL_DestroyTexture>> TexturePtr;
typedef std::unique_ptr<SDL_Window, SDLDeleter<SDL_Window, SDL_DestroyWindow>> WindowPtr;
typedef std::unique_ptr<SDL_PixelFormat, SDLDeleter<SDL_PixelFormat, SDL_FreeFormat>> PixelFormatPtr;
typedef std::unique_ptr<SDL_Palette, SDLDeleter<SDL_Palette, SDL_FreePalette>> PalettePtr;
typedef std::unique_ptr<TTF_Font, SDLDeleter<TTF_Font, TTF_CloseFont>> FontPtr;

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const {
        SDL_RWclose(src);
    }
};

typedef std::unique_ptr<SDL_RWops, RWCloseDeleter> RWPtr;

#endif

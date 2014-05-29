#ifndef SDL_UTILS_H_
#define SDL_UTILS_H_

#include <memory>

#include <SDL.h>

#include <game/sdl_error.h>

struct SDLInitializer final
{
    SDLInitializer(int flags) throw(sdl_error);
    ~SDLInitializer() throw();
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

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const {
        SDL_RWclose(src);
    }
};

typedef std::unique_ptr<SDL_RWops, RWCloseDeleter> RWPtr;

#endif

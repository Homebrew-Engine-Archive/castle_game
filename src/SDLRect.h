#ifndef SDLRECT_H_
#define SDLRECT_H_

#include <SDL2/SDL.h>

class SDLRect
{
    SDL_Rect *rect;
    
public:
    SDLRect();
    SDLRect(int w, int h);
    SDLRect(int x, int y, int w, int h);

    ~SDLRect();

    bool IsNull() const;

    const SDL_Rect* GetRect() const;
    SDL_Rect* GetRect();
};

#endif

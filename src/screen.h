#ifndef SCREEN_H_
#define SCREEN_H_

#include <SDL2/SDL.h>

class Screen
{
public:
    virtual void Draw(SDL_Renderer *renderer, const SDL_Rect &dstrect) = 0;
    virtual void OnMouseClick(const SDL_Event &event) = 0;
    virtual void OnKeyDown(const SDL_Event &event) = 0;
};

#endif

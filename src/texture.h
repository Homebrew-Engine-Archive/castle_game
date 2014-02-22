#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL2/SDL.h>
#include "renderer.h"

class Texture
{
public:
    virtual void Draw(Renderer &renderer, const SDL_Rect *srcrect, const SDL_Rect *dstrect) = 0;
};

class Sprite : public Texture
{
public:
    Sprite(Texture &texture, const SDL_Rect &rect);
    void Draw(Renderer &renderer, const SDL_Rect *srcrect, const SDL_Rect *dstrect);
};

#endif

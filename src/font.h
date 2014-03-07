#ifndef FONT_H_
#define FONT_H_

#include <memory>
#include <string>
#include "SDL.h"

class Font;

typedef size_t font_size_t;

#include "gm1.h"

class Font
{
public:
    virtual void PutGlyph(SDL_Renderer *renderer, int character, const SDL_Color &color, font_size_t size) = 0;
};

#endif

#ifndef FONT_H_
#define FONT_H_

#include <memory>
#include <string>
#include "SDL.h"

class Font;
class GM1Font;

typedef size_t font_size_t;

#include "gm1.h"

class Font
{
public:
    virtual void PutChar(const SDL_Renderer &renderer,
                         int character, const SDL_Color &color, font_size_t size) = 0;
};

class GM1Font : public Font
{
public:
    GM1Font(const gm1::Collection &gm1, const Surface &glyphs);
    
    void PutChar(const SDL_Renderer &renderer,
                 int character, const SDL_Color &color, font_size_t size);
};

#endif

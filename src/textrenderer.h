#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <cstdio>
#include <string>
#include <cstdarg>
#include <SDL2/SDL.h>

#include "font.h"
#include "renderer.h"

class TextRenderer
{
public:
    TextRenderer(Renderer &renderer, const Font &font);
    ~TextRenderer();

    void SetColor(const SDL_Color *color);

    void Printf(const char *fmt, ...);

    void PutString(const std::string &str);
    void PutString(const char *str);
    
};
    
#endif

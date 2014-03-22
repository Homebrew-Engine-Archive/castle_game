#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <memory>
#include <string>
#include <SDL.h>
#include "sdl_utils.h"

class Text;
class Font;

struct TextRenderer
{
    virtual bool CacheFont(const std::string &name, int size, const Font &font) = 0;
    virtual bool SetFont(const std::string &fontname, int size) = 0;
    virtual bool SetFontName(const std::string &fontname) = 0;
    virtual bool SetFontSize(int size) = 0;
    virtual void SetColor(const SDL_Color &color) = 0;
    virtual void SetCursor(const SDL_Point &cursor) = 0;
    virtual SDL_Rect CalculateTextRect(const std::string &str) const = 0;
    virtual void PutString(const std::string &str) = 0;
};

std::unique_ptr<TextRenderer> CreateTextRenderer(SDL_Renderer *renderer);

void PutText(const TextRenderer &renderer, const Text &text, const SDL_Rect &box);

#endif

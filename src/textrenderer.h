
#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <map>
#include <cstdarg>
#include <iostream>
#include <string>
#include "SDL.h"

class Font;
class GM1Font;
class Renderer;
class TextRenderer;

#include "gm1.h"
#include "tgx.h"
#include "font.h"

class TextRenderer
{
    Renderer *m_renderer;

    std::map<std::string, std::unique_ptr<Font>> m_fonts;
    
public:
    TextRenderer(Renderer *renderer);

    /**
     * Make font available for drawing under given name.
     * @param name      Future name.
     * @param font      Pointer to font object.
     * @param ...       Sizes which available to use with given font.
     *
     */
    void RegisterFont(const std::string &name, std::unique_ptr<Font> font, ...);

    /**
     * UnregisterFont not really used by the game, but present
     * for genericity purposes.
     */
    void UnregisterFont(const std::string &name);
    
    void SetFont(const Font &font);

    void SetSize(font_size_t size);

    void SetColor(const SDL_Color &color);
    
    int CalculateTextHeight(const std::string &str) const;
    
    int CalculateTextWidth(const std::string &str) const;

    /**
     * Puts only single line of text. The rest of the string will be
     * dropped.
     * @param str       Intent to be a string.
     * @param rect      Bounding rect of drawing area.
     *
     */
     
    void PutString(const std::string &str, const SDL_Rect &rect);
};

#include "renderer.h"

#endif

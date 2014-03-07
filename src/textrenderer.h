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

    const Font *m_font;
    SDL_Point m_cursor;
    SDL_Color m_color;
    font_size_t m_fontSize;

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
     * Use new font for next printing.
     * @param fontname  Font name to use.
     * @return          False if no such font and true otherwise.
     * 
     */
    bool SetFont(const std::string &fontname);

    void SetSize(font_size_t size);

    void SetColor(const SDL_Color &color);

    void SetCursor(const SDL_Point &cursor);
    
    int CalculateTextHeight(const std::string &str) const;
    
    int CalculateTextWidth(const std::string &str) const;

    /**
     * Puts single character 
     * @param character         Character which one need to put.
     * @param rect              Bounding rect of drawing area.
     *
     */
     
    void PutChar(int character);
};

#include "renderer.h"

#endif

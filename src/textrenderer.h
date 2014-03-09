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

#include "sdl_utils.h"
#include "gm1.h"
#include "tgx.h"
#include "font.h"

struct FontData
{
    font_size_t size;
    std::string fontname;
    std::unique_ptr<Font> font;
    FontData(font_size_t size, const std::string &fn, std::unique_ptr<Font> font);
};

class TextRenderer
{
    Renderer *m_renderer;
    const FontData *m_fontData;
    SDL_Point m_cursor;
    SDL_Point m_startCursor;
    SDL_Color m_color;
    int m_kerning;
    int m_lineSpacing;
    int m_lineHeight;
    
    std::vector<FontData> m_fonts;

    const FontData *GetBestMatch(const std::string &fn, font_size_t size, const FontData *lhs, const FontData *rhs);
    
public:
    TextRenderer(Renderer *renderer);

    /**
     * Make font available for drawing under given name.
     * @param name      Future name.
     * @param font      Pointer to font object.
     * @param size      Size of font.
     *
     */
    void RegisterFont(const std::string &name, font_size_t size, std::unique_ptr<Font> font);
    
    /**
     * Use new font for next printing.
     * If there is no font with same name, took font with the same name but smaller size.
     * If there is no font with same name and smaller size, took font with larger size.
     * If there is no font with this name, took any other font with the same size.
     * @param fontname  Font name to use.
     * @param size      Font size.
     * @return          False if is no font available at all and true otherwise.
     * 
     */
    bool SetFont(const std::string &fontname, font_size_t size);

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

    void PutNewline();
};

#include "renderer.h"

#endif

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

class TextRenderer;

#include <map>
#include <cstdarg>
#include <iostream>
#include <string>
#include "SDL.h"

#include "sdl_utils.h"
#include "gm1.h"
#include "tgx.h"
#include "font.h"

struct FontData
{
    std::string fontname;
    font_size_t size;
    Font font;
    TexturePtr texture;
    std::vector<SDL_Rect> partition;
};

class TextRenderer
{
    SDL_Renderer *m_renderer;
    const FontData *m_fontData;
    SDL_Color m_color;
    int m_cursorX;
    int m_cursorY;
    std::vector<FontData> m_fonts;

    TexturePtr CreateFontAtlas(const Font &font, std::vector<SDL_Rect> &partition) const;
    
    /**
     * Searches for appreciate rectangle in m_fontData->partition
     *
     * @param   character       Character code to search for.
     * @return                  Rectangle in font's texture.
     */
    SDL_Rect FindTextureSubrect(int character) const;

    /**
     * Searches for appreciate font data in m_fontData->glyphs
     *
     * @param   character       Character code to search for.
     * @return                  Glyphs data pointer or NULL.
     */
    const GlyphData *FindGlyphData(int character) const;
    
    const FontData *GetBestMatch(const std::string &fn, font_size_t size, const FontData *lhs, const FontData *rhs);
    
public:
    TextRenderer(SDL_Renderer *renderer);

    /**
     * Make font available for drawing under given name.
     *
     * @param name      Future name.
     * @param font      Pointer to font object.
     * @param size      Size of font.
     *
     * @return          True if no error occurred.
     */
    bool CacheFont(const std::string &name, font_size_t size, const Font &font);
    
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

    bool SetFontName(const std::string &fontname);

    bool SetFontSize(font_size_t size);

    void SetColor(const SDL_Color &color);

    void SetCursor(const SDL_Point &cursor);

    SDL_Rect CalculateTextRect(const std::string &str) const;
    
    /**
     * Puts single character 
     * @param character         Character which one need to put.
     * @param rect              Bounding rect of drawing area.
     *
     */
    void PutChar(int character);

    void PutLine(const std::string &str);

};

typedef std::unique_ptr<TextRenderer> TextRendererPtr;

#endif

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

class TextRenderer;

#include <string>
#include "SDL.h"
#include "sdl_utils.h"
#include "font.h"

class TextRenderer
{
private:
    struct TextRendererPimpl *m;
    
public:
    TextRenderer(SDL_Renderer *renderer);
    TextRenderer(const TextRenderer &that) = delete;
    TextRenderer &operator=(const TextRenderer &that) = delete;
    virtual ~TextRenderer();

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
     * Consequently puts each character in the string.
     *
     * @param str       String which we shall put.
     *
     */
    void PutString(const std::string &str);

};

typedef std::unique_ptr<TextRenderer> TextRendererPtr;

#endif

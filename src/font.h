#ifndef FONT_H_
#define FONT_H_

#include <memory>
#include <string>
#include "SDL.h"

class Font;
struct FontAtlasInfo;
typedef size_t font_size_t;

#include "sdl_utils.h"
#include "renderer.h"
#include "gm1.h"

class Font
{
public:
    virtual bool PutGlyph(Renderer *renderer, int character, const SDL_Point &point, const SDL_Color &color) const = 0;

    virtual SDL_Rect GetGlyphBounds(int character) const = 0;
};

class GM1Font : public Font
{
protected:
    TexturePtr m_atlas;
    std::vector<SDL_Rect> m_glyphData;

public:
    GM1Font(const CollectionAtlas &atlas, const std::vector<int> &alphabet, size_t skip, size_t count, int yoffset);

    bool PutGlyph(Renderer *renderer, int character, const SDL_Point &point, const SDL_Color &color) const;

    SDL_Rect GetGlyphBounds(int character) const;
};

struct FontAtlasInfo
{
    std::string filename;
    std::string name;
    font_size_t size;
    std::vector<char> alphabet;
    int skip;
    int count;
    int yoffset;
    int xoffset;
};

#endif

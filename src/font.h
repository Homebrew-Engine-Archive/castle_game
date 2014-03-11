#ifndef FONT_H_
#define FONT_H_

#include <cassert>
#include <array>
#include <map>
#include <memory>
#include <string>
#include "SDL.h"

#include "atlas.h"
#include "sdl_utils.h"
#include "collection.h"
#include "gm1.h"

class Font
{
    struct GlyphData
    {
        Surface surface;
        int width;
        int height;
        int character;
        int yoffset;
        int xoffset;
    };
    
    std::vector<int> m_asciiList;
    std::array<size_t, 256> m_asciiIndex;
    std::vector<Surface> m_surfaces;
    std::vector<int> m_yoffsets;
    
public:
    Font();
    Font(const CollectionData &data, const std::vector<int> &alphabet, int skip);
    TexturePtr CreateFontAtlas(SDL_Renderer *renderer, std::vector<SDL_Rect> &partition) const;
    int GetGlyphYOffset(size_t index) const;
    Surface GetGlyphSurface(size_t index) const;
    size_t GetGlyphIndex(int character) const;

};

typedef std::unique_ptr<Font> FontPtr;

Surface DecodeGM1Glyph(const Surface &src);

#endif

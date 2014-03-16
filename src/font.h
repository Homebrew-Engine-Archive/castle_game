#ifndef FONT_H_
#define FONT_H_

#include <array>
#include <map>
#include <memory>
#include <string>
#include "SDL.h"

#include "atlas.h"
#include "sdl_utils.h"
#include "collection.h"
#include "gm1.h"

struct GlyphData
{
    Surface face;
    int xadvance;
    int xbearing;
    int yadvance;
    int ybearing;
    int hbox;
    int vbox;
};

class Font
{
    std::vector<GlyphData> m_glyphs;
    std::vector<size_t> m_ascii;
    
    int m_fontWidth;
    int m_fontHeight;

public:
    Font() = default;
    Font(const CollectionData &data, const std::vector<int> &alphabet, int skip);
    const GlyphData *FindGlyph(int character) const;
    const GlyphData *GetGlyph(size_t index) const;
    size_t GetGlyphIndex(int character) const;
    std::vector<GlyphData> GetGlyphList() const;
    
    inline int FontHeight() const { return m_fontHeight; }
    inline int FontWidth() const { return m_fontWidth; }
    inline int LineSpacing() const { return 0; };
    inline int Kerning() const { return 0; };

    bool AddGlyph(int character, const GlyphData &glyph);

};

int GlyphWidth(const GlyphData &glyph);
int GlyphHeight(const GlyphData &glyph);

#endif

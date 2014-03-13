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

    void InitNonPrintableAscii(Surface, int, int);

public:
    Font();
    Font(const CollectionData &data, const std::vector<int> &alphabet, int skip);
    const GlyphData *FindGlyph(int character) const;
    const GlyphData *GetGlyph(size_t index) const;
    size_t GetGlyphIndex(int character) const;
    std::vector<GlyphData> GetGlyphList() const;
    inline int GetHeight() const { return m_fontHeight; }
    inline int GetWidth() const { return m_fontWidth; }
    inline int GetLineSpacing() const { return 0; };
    inline int GetKerning() const { return 0; };

    bool AddGlyph(int character, const GlyphData &glyph);

};

typedef std::unique_ptr<Font> FontPtr;

#endif

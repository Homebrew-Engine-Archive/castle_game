#ifndef FONT_H_
#define FONT_H_

#include <vector>
#include "sdl_utils.h"
#include "surface.h"

class CollectionData;

namespace Render
{

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
        std::vector<GlyphData> mGlyphs;
        std::vector<size_t> mAscii;
        int mFontWidth;
        int mFontHeight;

    public:
        Font();
        const GlyphData *FindGlyph(int character) const;
        const GlyphData *GetGlyph(size_t index) const;
        size_t GetGlyphIndex(int character) const;
        std::vector<GlyphData> GetGlyphList() const;
    
        int Height() const;
        int Width() const;
        int LineSpacing() const;
        int Kerning() const;
        bool AddGlyph(int character, const GlyphData &glyph);

    };

    int GlyphWidth(const GlyphData &glyph);
    int GlyphHeight(const GlyphData &glyph);

    Font MakeFont(const CollectionData &data, const std::vector<int> &alphabet, int skip);
    
} // namespace Render

#endif

#include "font.h"

#include <stdexcept>
#include <sstream>

#include <game/surface.h>
#include <game/gm1.h>
#include <game/collection.h>
#include <boost/current_function.hpp>

namespace
{

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
        
    void AddNonPrintableGlyphs(Surface face, int xadvance, int ybearing, Render::Font &font)
    {
        const int NonPrintableLast = '!';
        for(int character = 0; character < NonPrintableLast; ++character) {
            Render::GlyphData glyph;
            glyph.face = face;
            glyph.hbox = 0;
            glyph.vbox = 0;
            glyph.xadvance = xadvance;
            glyph.xbearing = 0;
            glyph.yadvance = 0;
            glyph.ybearing = ybearing;
            if(!font.AddGlyph(character, glyph)) {
                Fail(BOOST_CURRENT_FUNCTION, "Unable to add non-printable glyph");
            }
        }
    }
    
    int GlyphWidth(const Render::GlyphData &glyph)
    {
        return glyph.xadvance;
    }

    int GlyphHeight(const Render::GlyphData &glyph)
    {
        return glyph.vbox;
    }

}

namespace Render
{

    Font::Font()
        : mGlyphs()
        , mAscii(256)
        , mFontWidth(0)
        , mFontHeight(0)
    { }

    int Font::Height() const
    {
        return mFontHeight;
    }

    int Font::Width() const
    {
        return mFontWidth;
    }

    int Font::LineSpacing() const
    {
        return 0;
    }

    int Font::Kerning() const
    {
        return 0;
    }

    Font MakeFont(const CollectionData &data, const std::vector<int> &alphabet, int skip)
    {
        Font font;
        AddNonPrintableGlyphs(Surface(), data.header.anchorX, 0, font);
    
        auto entry = data.entries.begin();
        std::advance(entry, skip);
        for(int character : alphabet) {
            if(entry >= data.entries.end()) {
                Fail(BOOST_CURRENT_FUNCTION, "Entry index out of bounds");
            }
        
            GlyphData glyph;
        
            // Copy original surface and convert to argb32
            if(entry->surface.Null()) {
                glyph.face = Surface();
                glyph.hbox = 0;
                glyph.vbox = 0;
            } else {
                glyph.face = entry->surface;
                glyph.hbox = glyph.face->w;
                glyph.vbox = glyph.face->h;
            }
        
            glyph.xadvance = glyph.hbox;
            glyph.xbearing = 0;
            glyph.yadvance = 0;
            glyph.ybearing = entry->header.tileY;

            if(!font.AddGlyph(character, glyph)) {
                Fail(BOOST_CURRENT_FUNCTION, "Unable to add glyph into font");
            }

            ++entry;
        }

        return font;
    }

    bool Font::AddGlyph(int character, const GlyphData &glyph)
    {
        if(character < (int)mAscii.size()) {
            mAscii[character] = mGlyphs.size();
            mFontWidth = std::max(mFontWidth, glyph.hbox);
            mFontHeight = std::max(mFontHeight, glyph.vbox);
            mGlyphs.push_back(glyph);
            return true;
        }

        return false;
    }

    const GlyphData *Font::FindGlyph(int character) const
    {
        size_t idx = GetGlyphIndex(character);
        return GetGlyph(idx);
    }

    const GlyphData *Font::GetGlyph(size_t idx) const
    {
        if(idx < mGlyphs.size()) {
            return &mGlyphs[idx];
        }

        return NULL;
    }

    std::vector<GlyphData> Font::GetGlyphList() const
    {
        return mGlyphs;
    }

    size_t Font::GetGlyphIndex(int character) const
    {
        if(character < (int)mAscii.size())
            return mAscii[character];
    
        return mGlyphs.size();
    }

} // namespace Render

namespace Render
{
    void LoadFonts(Render::GraphicsManager&)
    {
        
    }
}

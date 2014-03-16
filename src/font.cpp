#include "font.h"

static Surface DecodeGM1Glyph(const Surface &src)
{
    Surface rgb32 = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_ARGB8888, NO_FLAGS);
    ThrowSDLError(rgb32);

    // Swap green channel with alpha channel, so
    // fully white pixels remain unchanged, but
    // magenta-colored pixels become alpha-blended

    // This way we are also reaching font-aliasing
    auto swap_green_alpha = [](Uint8 r, Uint8 g, Uint8 b, Uint8) {
        return SDL_Color { r, 255, b, g };
    };
    
    MapSurface(rgb32, swap_green_alpha);

    return rgb32;
}

Font::Font(const CollectionData &data, const std::vector<int> &alphabet, int skip)
    : m_ascii(256)
    , m_fontWidth(0)
    , m_fontHeight(0)
{
    InitNonPrintableAscii(Surface(), data.header.anchorX, 0);
    
    auto entry = data.entries.begin();
    std::advance(entry, skip);
    for(int character : alphabet) {
        if(entry >= data.entries.end()) {
            throw std::runtime_error("Entry index out of bounds");
        }
        
        GlyphData glyph;
        
        // Copy original surface and convert to argb32
        if(entry->surface.Null()) {
            glyph.face = Surface();
            glyph.hbox = 0;
            glyph.vbox = 0;
        } else {
            glyph.face = DecodeGM1Glyph(entry->surface);
            glyph.hbox = glyph.face->w;
            glyph.vbox = glyph.face->h;
        }
        
        glyph.xadvance = glyph.hbox;
        glyph.xbearing = 0;
        glyph.yadvance = 0;
        glyph.ybearing = entry->header.tileY;

        if(!AddGlyph(character, glyph)) {
            throw std::runtime_error("Unable to add glyph into font");
        }

        ++entry;
    }
}

bool Font::AddGlyph(int character, const GlyphData &glyph)
{
    if(character < (int)m_ascii.size()) {
        m_ascii[character] = m_glyphs.size();
        m_fontWidth = std::max(m_fontWidth, glyph.hbox);
        m_fontHeight = std::max(m_fontHeight, glyph.vbox);
        m_glyphs.push_back(glyph);
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
    if(idx < m_glyphs.size()) {
        return &m_glyphs[idx];
    }

    return NULL;
}

std::vector<GlyphData> Font::GetGlyphList() const
{
    return m_glyphs;
}

size_t Font::GetGlyphIndex(int character) const
{
    if(character < (int)m_ascii.size())
        return m_ascii[character];
    
    return m_glyphs.size();
}

void Font::InitNonPrintableAscii(Surface face, int xadvance, int ybearing)
{
    const int NonPrintableLast = '!';
    for(int character = 0; character < NonPrintableLast; ++character) {
        GlyphData glyph;
        glyph.face = face;
        glyph.hbox = 0;
        glyph.vbox = 0;
        glyph.xadvance = xadvance;
        glyph.xbearing = 0;
        glyph.yadvance = 0;
        glyph.ybearing = ybearing;
        if(!AddGlyph(character, glyph)) {
            throw std::runtime_error("Unable to add non-printable glyph");
        }
    }
}

int GlyphWidth(const GlyphData &glyph)
{
    return glyph.xadvance;
}

int GlyphHeight(const GlyphData &glyph)
{
    return glyph.vbox;
}

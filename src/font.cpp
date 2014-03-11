#include "font.h"

Surface DecodeGM1Glyph(const Surface &src)
{
    Surface rgb32 = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_ARGB8888, NO_FLAGS);
    ThrowSDLError(rgb32);

    // Swap green channel with alpha channel, so
    // fully white pixels remain unchanged, but
    // magenta-colored pixels become alpha-blended

    // That how we reach font-aliasing
    auto swap_green_alpha = [](Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        return SDL_Color { r, 255, b, g };
    };
    
    MapSurface(rgb32, swap_green_alpha);

    return rgb32;
}

Font::Font()
{
}

Font::Font(const CollectionData &data, const std::vector<int> &alphabet, int skip)
    : m_asciiList(alphabet)
    , m_asciiIndex {{0}}
{
    for(size_t index = 0; index < m_asciiList.size(); ++index) {
        m_asciiIndex[m_asciiList[index]] = index;
    }
    
    for(int character : m_asciiList) {
        const CollectionEntry &entry = data.entries.at(skip);

        m_yoffsets.push_back(entry.header.tileY);

        // Copy original surface with format conversion to rgb32
        Surface glyphFace = DecodeGM1Glyph(entry.surface);
        ThrowSDLError(glyphFace);
        m_surfaces.push_back(glyphFace);

        ++skip;
    }

    m_asciiList.push_back(' ');
    m_asciiIndex[' '] = alphabet.size();
    m_surfaces.push_back(Surface());
    m_yoffsets.push_back(0);
}

size_t Font::GetGlyphIndex(int character) const
{
    return m_asciiIndex.at(character);
}

int Font::GetGlyphYOffset(size_t index) const
{
    return m_yoffsets.at(index);
}

Surface Font::GetGlyphSurface(size_t index) const
{
    return m_surfaces.at(index);
}

TexturePtr Font::CreateFontAtlas(SDL_Renderer *renderer, std::vector<SDL_Rect> &partition) const
{
    SDL_RendererInfo info;
    ThrowSDLError(
        SDL_GetRendererInfo(renderer, &info));
    
    Surface atlas = MakeSurfaceAtlas(
        m_surfaces, partition, info.max_texture_width, info.max_texture_height);
    ThrowSDLError(atlas);
    
    TexturePtr ptr = TexturePtr(
        SDL_CreateTextureFromSurface(renderer, atlas));
    ThrowSDLError(ptr);
    
    return ptr;
}

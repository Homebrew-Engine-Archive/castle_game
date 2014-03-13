#include "textrenderer.h"

TextRenderer::TextRenderer(SDL_Renderer *renderer)
    : m_renderer(renderer)
    , m_fontData(NULL)
    , m_color { 0, 0, 0, 0 }
    , m_cursorX(0)
    , m_cursorY(0)
{
}

static SDL_Rect GetGlyphFaceBox(const GlyphData &glyph, int x, int y)
{
    return MakeRect(
        x + glyph.xbearing,
        y - glyph.ybearing,
        glyph.hbox,
        glyph.vbox);
}

static SDL_Rect GetGlyphAdvanceBox(const GlyphData &glyph, int x, int y)
{
    return MakeRect(
        x + glyph.xbearing,
        y - glyph.ybearing,
        glyph.xadvance,
        glyph.vbox);
}

void TextRenderer::PutLine(const std::string &str)
{
    for(int character : str) {
        PutChar(character);
    }
}

void TextRenderer::PutChar(int character)
{
    if(m_fontData != NULL) {
        SDL_Texture *texture = m_fontData->texture.get();
        SDL_SetTextureColorMod(texture, m_color.r, m_color.g, m_color.b);
        SDL_SetTextureAlphaMod(texture, m_color.a);
        
        const GlyphData *glyphData = FindGlyphData(character);
        if(glyphData != NULL) {
            SDL_Rect srcRect = FindTextureSubrect(character);
            SDL_Rect dstRect = GetGlyphFaceBox(*glyphData, m_cursorX, m_cursorY);
            ThrowSDLError(
                SDL_RenderCopy(m_renderer, texture, &srcRect, &dstRect));
            SDL_Rect advanceBox = GetGlyphAdvanceBox(*glyphData, m_cursorX, m_cursorY);
            m_cursorX += advanceBox.w;
        }
    }
}

SDL_Rect TextRenderer::CalculateTextRect(const std::string &str) const
{
    SDL_Rect bounds = MakeEmptyRect();
    if(m_fontData != NULL) {
        int x = m_cursorX;
        for(int character : str) {
            const GlyphData *glyphData = FindGlyphData(character);
            if(glyphData != NULL) {
                SDL_Rect glyphRect = GetGlyphAdvanceBox(*glyphData, x, m_cursorY);
                SDL_UnionRect(&glyphRect, &bounds, &bounds);
                x += glyphRect.w;
            }
        }
    }
    return bounds;
}

const GlyphData *TextRenderer::FindGlyphData(int character) const
{
    if(m_fontData != NULL) {
        return m_fontData->font.FindGlyph(character);
    }
    return NULL;
}

SDL_Rect TextRenderer::FindTextureSubrect(int character) const
{
    if(m_fontData != NULL) {
        size_t idx = m_fontData->font.GetGlyphIndex(character);
        if(idx < m_fontData->partition.size())
            return m_fontData->partition[idx];
    }
    return MakeEmptyRect();
}

bool TextRenderer::CacheFont(const std::string &name, font_size_t size, const Font &font)
{
    FontData fontData;
    fontData.fontname = name;
    fontData.size = size;
    fontData.font = font;
    
    fontData.texture =
        std::move(
            CreateFontAtlas(font, fontData.partition));
    if(!fontData.texture) {
        std::cerr << "Failed to create font atlas" << std::endl;
        return false;
    }

    m_fonts.push_back(std::move(fontData));
    return true;
}

TexturePtr TextRenderer::CreateFontAtlas(const Font &font, std::vector<SDL_Rect> &partition) const
{
    std::vector<Surface> surfaces;
    for(const GlyphData &glyph : font.GetGlyphList()) {
        surfaces.push_back(glyph.face);
    }

    SDL_RendererInfo info;
    ThrowSDLError(
        SDL_GetRendererInfo(m_renderer, &info));
    int maxWidth = info.max_texture_width;
    int maxHeight = info.max_texture_height;
    
    Surface atlas = MakeSurfaceAtlas(
        surfaces, partition, maxWidth, maxHeight);
    ThrowSDLError(atlas);
    
    TexturePtr ptr = TexturePtr(
        SDL_CreateTextureFromSurface(m_renderer, atlas));
    ThrowSDLError(ptr);
    
    return ptr;
}

void TextRenderer::SetCursor(const SDL_Point &cursor)
{
    m_cursorX = cursor.x;
    m_cursorY = cursor.y;
}

void TextRenderer::SetColor(const SDL_Color &color)
{
    m_color = color;
}

bool TextRenderer::SetFont(const std::string &fontname, font_size_t size)
{
    if(m_fontData != NULL) {
        if((m_fontData->fontname == fontname) && (m_fontData->size == size))
            return true;
    }    
    m_fontData = NULL;
    for(const auto &font : m_fonts) {
        m_fontData = GetBestMatch(fontname, size, m_fontData, &font);
    }
    if(m_fontData == NULL) {
        return false;
    }
    return true;
}

bool TextRenderer::SetFontSize(font_size_t size)
{
    std::string fontName;
    if(m_fontData != NULL) {
        fontName = m_fontData->fontname;
    }
    return SetFont(fontName, size);
}

bool TextRenderer::SetFontName(const std::string &name)
{
    font_size_t size = 0;    
    if(m_fontData != NULL) {
        size = m_fontData->size;
    }    
    return SetFont(name, size);
}

const FontData *TextRenderer::GetBestMatch(const std::string &fn, font_size_t size, const FontData *lhs, const FontData *rhs)
{
    if(lhs == NULL)
        return rhs;
    if(rhs == NULL)
        return lhs;
    if((lhs->fontname == fn) && (rhs->fontname != fn))
        return lhs;
    if((lhs->fontname != fn) && (rhs->fontname == fn))
        return rhs;
    // Best size is the largest one but not greater than `size'
    // or else the smallest one
    if(lhs->size > rhs->size) {
        if(lhs->size > size) {
            return rhs;
        } else {
            return lhs;
        }
    } else {
        if(rhs->size > size) {
            return rhs;
        } else {
            return lhs;
        }
    }
}

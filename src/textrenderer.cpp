#include "textrenderer.h"

TextRenderer::TextRenderer(SDL_Renderer *renderer)
    : m_renderer(renderer)
    , m_fontData(NULL)
    , m_cursor { 0, 0 }
    , m_startCursor(m_cursor)
    , m_color { 0, 0, 0, 0 }
    , m_kerning(2)
    , m_lineSpacing(2)
    , m_lineHeight(0)
{
}

void TextRenderer::PutChar(int character)
{
    if(m_fontData != NULL) {
        SDL_Texture *texture = m_fontData->texture.get();
        SDL_SetTextureColorMod(texture, m_color.r, m_color.g, m_color.b);
        SDL_SetTextureAlphaMod(texture, m_color.a);

        size_t index = m_fontData->font.GetGlyphIndex(character);
        int yoffset = m_fontData->font.GetGlyphYOffset(index);
        
        SDL_Rect srcRect = m_fontData->partition.at(index);
        SDL_Rect dstRect = MakeRect(
            m_cursor.x, m_cursor.y - yoffset,
            srcRect.w, srcRect.h);
        ThrowSDLError(
            SDL_RenderCopy(m_renderer, texture, &srcRect, &dstRect));
        m_cursor.x += dstRect.w + m_kerning;
    }
}

void TextRenderer::PutNewline()
{
    m_cursor = m_startCursor;
    m_cursor.y += m_lineHeight + m_lineSpacing;
}

SDL_Rect TextRenderer::CalculateTextRect(const std::string &str) const
{
    SDL_Rect bounds = MakeEmptyRect();
    if(m_fontData != NULL) {
        int x = m_cursor.x;
        for(int character : str) {
            size_t idx = m_fontData->font.GetGlyphIndex(character);
            int yoffset = m_fontData->font.GetGlyphYOffset(idx);
            SDL_Rect srcRect = m_fontData->partition.at(idx);

            SDL_Rect dstRect = MakeRect(
                x, m_cursor.y - yoffset,
                srcRect.w, srcRect.h);
            SDL_UnionRect(&dstRect, &bounds, &bounds);
            x += dstRect.w + m_kerning;
        }
    }
    return bounds;
}

void TextRenderer::RegisterFont(const std::string &name, font_size_t size, const Font &font)
{
    FontData fontData;
    fontData.fontname = name;
    fontData.size = size;
    fontData.font = font;
    fontData.texture = std::move(
        font.CreateFontAtlas(m_renderer, fontData.partition));
    ThrowSDLError(fontData.texture);
    
    m_fonts.push_back(std::move(fontData));
}

void TextRenderer::SetCursor(const SDL_Point &cursor)
{
    m_cursor = cursor;
    m_lineHeight = 0;
    m_startCursor = cursor;
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

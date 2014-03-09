#include "textrenderer.h"

TextRenderer::TextRenderer(Renderer *renderer)
    : m_renderer(renderer)
    , m_fontData(NULL)
    , m_cursor { 0, 0 }
    , m_startCursor(m_cursor)
    , m_color { 0, 0, 0, 0 }
    , m_kerning(-2)
    , m_lineSpacing(2)
    , m_lineHeight(0)
{
}

void TextRenderer::PutChar(int character)
{
    if(m_fontData != NULL) {
        const Font *font = m_fontData->font.get();
        font->PutGlyph(m_renderer, character, m_cursor, m_color);

        SDL_Rect glyphBounds = font->GetGlyphBounds(character);
        m_cursor.x = glyphBounds.w + m_kerning;

        m_lineHeight = std::max(m_lineHeight, glyphBounds.h);
    }
}

void TextRenderer::PutNewline()
{
    m_cursor = m_startCursor;
    m_cursor.y += m_lineHeight + m_lineSpacing;
}

int TextRenderer::CalculateTextWidth(const std::string &str) const
{
    return 0;
}

int TextRenderer::CalculateTextHeight(const std::string &str) const
{
    return 0;
}

void TextRenderer::RegisterFont(const std::string &name, font_size_t size, std::unique_ptr<Font> font)
{
    m_fonts.emplace_back(size, name, std::move(font));
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

FontData::FontData(font_size_t size_, const std::string &fn_, std::unique_ptr<Font> font_)
    : size(size_)
    , fontname(fn_)
    , font(std::move(font_))
{ }

#include "textrenderer.h"

TextRenderer::TextRenderer(Renderer *renderer)
    : m_renderer(renderer)
    , m_font(NULL)
    , m_cursor { 0, 0 }
    , m_color { 0, 0, 0, 0 }
    , m_fontSize(0)
{
}

void TextRenderer::PutChar(int character)
{
    if(m_font != NULL) {
        std::cout << (char)character;
    }
}

int TextRenderer::CalculateTextWidth(const std::string &str) const
{
    UNUSED(str);
    return 0;
}

int TextRenderer::CalculateTextHeight(const std::string &str) const
{
    UNUSED(str);
    return 0;
}

void TextRenderer::RegisterFont(const std::string &name, std::unique_ptr<Font> font, ...)
{
    m_fonts.insert(
        std::make_pair(name, std::move(font)));
}

void TextRenderer::SetCursor(const SDL_Point &cursor)
{
    m_cursor = cursor;
}

void TextRenderer::SetColor(const SDL_Color &color)
{
    m_color = color;
}

void TextRenderer::SetSize(font_size_t size)
{
    m_fontSize = size;
}

bool TextRenderer::SetFont(const std::string &fontname)
{
    auto searchResult = m_fonts.find(fontname);
    if(searchResult != m_fonts.end()) {
        m_font = searchResult->second.get();
        return true;
    } else {
        m_font = NULL;
        std::cerr << "No font with this name: "
                  << fontname << std::endl;
        return false;
    }
}

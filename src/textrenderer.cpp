#include "textrenderer.h"

TextRenderer::TextRenderer(Renderer *renderer)
    : m_renderer(renderer)
{
}

void TextRenderer::PutString(const std::string &str, const SDL_Rect &rect)
{
    UNUSED(str);
    UNUSED(rect);
}

int TextRenderer::CalculateTextWidth(const std::string &str) const
{
    UNUSED(str);
    return 0;
}

int TextRenderer::CalculateTextHeight(const std::string &str) const
{
    UNUSED(str);
    return str.length();
}

void TextRenderer::RegisterFont(const std::string &name, std::unique_ptr<Font> font, ...)
{
    m_fonts.insert(
        std::make_pair(name, std::move(font)));
}

void TextRenderer::UnregisterFont(const std::string &name)
{
    m_fonts.erase(
        m_fonts.find(name));
}

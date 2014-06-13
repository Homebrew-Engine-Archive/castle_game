#include "textrenderer.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

#include <SDL_ttf.h>
#include <SDL.h>

#include <game/point.h>
#include <game/surface.h>
#include <game/fontmanager.h>

namespace Render
{
    TextRenderer::TextRenderer(Surface &surface)
        : mSurface(surface)
        , mTextFont(Render::RegularFont, 10)
        , mCurrentFont(nullptr)
    {
    }

    void TextRenderer::PutString(const std::string &str, const Rect &textArea, const Color &facecolor)
    {
    }
    
    const Rect TextRenderer::TextSize(const std::string &str) const
    {
        return Rect();
    }
    
    void TextRenderer::SetFont(const core::Font &font)
    {
    }

    const core::Font TextRenderer::GetFont() const
    {
        return mTextFont;
    }
} // namespace Render

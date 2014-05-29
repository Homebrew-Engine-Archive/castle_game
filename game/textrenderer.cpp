#include "textrenderer.h"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>

#include <SDL.h>

#include <game/point.h>
#include <game/surface.h>
#include <game/fontmanager.h>

namespace Render
{
    TextRenderer::TextRenderer(Surface &surface)
        : mSurface(surface)
        , mColor(0, 0, 0, 0)
        , mCursor(0, 0)
        , mCursorMode(CursorMode::BaseLine)
        , mClipBox(0, 0, 0, 0)
        , mFontStyle(FontStyle_Normal)
        , mFontName()
        , mFontSize()
        , mCurrentFont(nullptr)
    {
        SetFont(Render::FontStronghold, 10);
    }

    Point TextRenderer::GetTopLeftBoxPoint() const
    {
        switch(mCursorMode) {
        case CursorMode::BaseLine:
            return mCursor - Point(0, TTF_FontAscent(mCurrentFont));

        case CursorMode::BottomLeft:
            return mCursor - Point(0, TTF_FontHeight(mCurrentFont));

        case CursorMode::TopLeft:
        default:
            return mCursor;
        }
    }
    
    void TextRenderer::PutRenderedString(Surface &text)
    {
        if(SDL_SetSurfaceAlphaMod(text, mColor.a) < 0) {
            throw sdl_error();
        }
        
        if(SDL_SetSurfaceColorMod(text, mColor.r, mColor.g, mColor.b) < 0) {
            throw sdl_error();
        }

        const Rect dstRect(GetTopLeftBoxPoint(), text->w, text->h);
                
        BlitSurface(text, Rect(text), mSurface, dstRect);
        mCursor.x += text->w;
    }
    
    void TextRenderer::PutString(const std::string &str)
    {
        if(str.empty()) {
            return;
        }
        
        /** Real color will be choosen by SDL_SetSurfaceColorMod **/
        const Color glyphColor = Colors::White;

        TTF_SetFontStyle(mCurrentFont, mFontStyle);
        
        Surface textSurface;
        textSurface = TTF_RenderUTF8_Blended(mCurrentFont, str.c_str(), glyphColor);
        
        if(!textSurface) {
            throw ttf_error();
        }

        PutRenderedString(textSurface);
    }

    const Rect TextRenderer::CalculateTextRect(const std::string &str) const
    {
        int width;
        int height;
        
        if(TTF_SizeText(mCurrentFont, str.c_str(), &width, &height) < 0) {
            throw ttf_error();
        }
        return Rect(GetTopLeftBoxPoint(), width, height);
    }

    void TextRenderer::SetColor(const Color &color)
    {
        mColor = color;
    }

    void TextRenderer::SetCursorMode(CursorMode mode)
    {
        mCursorMode = mode;
    }
    
    void TextRenderer::SetFont(const std::string &fontName, int fontSize)
    {
        mFontName = fontName;
        mFontSize = fontSize;
        mCurrentFont = Render::FindFont(fontName, fontSize);
        if(mCurrentFont == nullptr) {
            throw std::runtime_error("no fonts available");
        }
    }

    void TextRenderer::SetClipBox(const Rect &clipbox)
    {
        mClipBox = clipbox;
    }

    void TextRenderer::SetFontStyle(int style)
    {
        mFontStyle = style;
    }
    
    const std::string TextRenderer::FontName() const
    {
        return mFontName;
    }

    int TextRenderer::FontSize() const
    {
        return mFontSize;
    }

    const Point TextRenderer::GetCursorPos() const
    {
        return mCursor;
    }
    
    void TextRenderer::SetCursorPos(Point pos)
    {
        mCursor = pos;
    }
    
} // namespace Render

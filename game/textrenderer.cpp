#include "textrenderer.h"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>

#include <SDL.h>

#include <game/point.h>

#include <boost/current_function.hpp>

#include <game/surface.h>
#include <game/exception.h>

namespace
{
    std::basic_string<Uint16> ToUint16String(const std::basic_string<char16_t> &sourceString)
    {
        std::basic_string<Uint16> targetString;
        targetString.reserve(sourceString.length());
        
        for(char16_t c : sourceString) {
            targetString.push_back(c);
        }
        
        return targetString;
    }
}

namespace Render
{
    TextRenderer::TextRenderer(Surface &surface)
        : mSurface(surface)
        , mColor(0, 0, 0, 0)
        , mCursor(0, 0)
        , mCursorMode(CursorMode::BaseLine)
        , mCurrentFont(nullptr)
        , mClipBox(0, 0, 0, 0)
        , mFontStyle(FontStyle_Normal)
    { }

    Point TextRenderer::GetTopLeftBoxPoint() const
    {
        CheckFontIsSet();
       
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
        CheckFontIsSet();
        
        if(SDL_SetSurfaceAlphaMod(text, mColor.a) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        
        if(SDL_SetSurfaceColorMod(text, mColor.r, mColor.g, mColor.b) < 0) {
            throw std::runtime_error(SDL_GetError());
        }

        Rect dstRect(GetTopLeftBoxPoint(), text->w, text->h);
                
        BlitSurface(text, Rect(text), mSurface, dstRect);
        Translate(text->w, 0);
    }
    
    void TextRenderer::PutString(const std::string &str)
    {
        CheckFontIsSet();
        
        /** Real color will be choosen by SDL_SetSurfaceColorMod **/
        const Color glyphColor = Color::White();
        
        Surface textSurface = TTF_RenderUTF8_Blended(mCurrentFont, str.c_str(), glyphColor);
        PutRenderedString(textSurface);
    }

    void TextRenderer::PutString(const std::u16string &str)
    {
        CheckFontIsSet();

        /** Real color will be choosen by SDL_SetSurfaceColorMod **/
        const Color glyphColor = Color::White();
        
        std::basic_string<Uint16> uint16Str = ToUint16String(str);
        Surface textSurface = TTF_RenderUNICODE_Blended(mCurrentFont, uint16Str.c_str(), glyphColor);
        PutRenderedString(textSurface);
    }
    
    Rect TextRenderer::CalculateTextRect(const std::string &str) const
    {
        CheckFontIsSet();
        
        int width;
        int height;
        
        if(TTF_SizeText(mCurrentFont, str.c_str(), &width, &height) < 0) {
            throw std::runtime_error(TTF_GetError());
        }
        return Rect(GetTopLeftBoxPoint(), width, height);
    }

    Rect TextRenderer::CalculateTextRect(const std::u16string &str) const
    {
        CheckFontIsSet();
        
        int width;
        int height;

        std::basic_string<Uint16> uint16Str = ToUint16String(str);
        
        if(TTF_SizeUNICODE(mCurrentFont, uint16Str.c_str(), &width, &height) < 0) {
            throw std::runtime_error(TTF_GetError());
        }
        return Rect(GetTopLeftBoxPoint(), width, height);
    }
    
    void TextRenderer::LoadIdentity()
    {
        mCursor = Point(0, 0);
    }
    
    void TextRenderer::Translate(int dx, int dy)
    {
        mCursor += Point(dx, dy);
    }

    void TextRenderer::SetColor(const Color &color)
    {
        mColor = color;
    }

    void TextRenderer::SetCursorMode(CursorMode mode)
    {
        mCursorMode = mode;
    }
    
    void TextRenderer::SetFont(TTF_Font *font)
    {
        mCurrentFont = font;
    }

    void TextRenderer::SetClipBox(const Rect &clipbox)
    {
        mClipBox = clipbox;
    }

    void TextRenderer::SetFontStyle(int style)
    {
        mFontStyle = style;
    }
    
    TTF_Font* TextRenderer::GetFont()
    {
        return mCurrentFont;
    }

    void TextRenderer::CheckFontIsSet() const
    {
        if(mCurrentFont == nullptr) {
            throw std::runtime_error("current font is null");
        }
    }
} // namespace Render

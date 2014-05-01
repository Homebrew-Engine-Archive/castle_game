#include "textrenderer.h"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>

#include <SDL.h>

#include <boost/current_function.hpp>

#include <game/make_unique.h>

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

    static SDL_Color White {255, 255, 255, 255};
}

namespace Render
{
    TextRenderer::TextRenderer(Surface &surface)
        : mSurface(surface)
        , mColor {0, 0, 0, 0}
        , mCursorX(0)
        , mCursorY(0)
    { }

    void TextRenderer::PutChar(int character)
    {

    }    

    SDL_Point TextRenderer::GetTopLeftBoxPoint() const
    {
        SDL_Point cursor { 0, 0 };
        switch(mCursorMode) {
        case CursorMode::BaseLine:
            cursor = MakePoint(mCursorX, mCursorY - TTF_FontAscent(mCurrentFont));
            break;
        case CursorMode::BottomLeft:
            cursor = MakePoint(mCursorX, mCursorY - TTF_FontHeight(mCurrentFont));
            break;
        case CursorMode::TopLeft:
        default:
            cursor = MakePoint(mCursorX, mCursorY);
            break;
        }
        return cursor;
    }
    
    void TextRenderer::PutRenderedString(Surface &text)
    {
        if(SDL_SetSurfaceAlphaMod(text, mColor.a) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        
        if(SDL_SetSurfaceColorMod(text, mColor.r, mColor.g, mColor.b) < 0) {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_Rect dstRect = MakeRect(GetTopLeftBoxPoint(), text->w, text->h);
                
        SDL_BlitSurface(text, NULL, mSurface, &dstRect);
        Translate(text->w, 0);
    }
    
    void TextRenderer::PutString(const std::string &str)
    {
        Surface textSurface = TTF_RenderUTF8_Blended(mCurrentFont, str.c_str(), White);
        PutRenderedString(textSurface);
    }

    void TextRenderer::PutString(const std::u16string &str)
    {
        std::basic_string<Uint16> uint16Str = ToUint16String(str);
        Surface textSurface = TTF_RenderUNICODE_Blended(mCurrentFont, uint16Str.c_str(), White);
        PutRenderedString(textSurface);
    }
    
    SDL_Rect TextRenderer::CalculateTextRect(const std::string &str) const
    {
        SDL_Rect size = MakeRect(GetTopLeftBoxPoint(), 0, 0);
        
        if(TTF_SizeText(mCurrentFont, str.c_str(), &size.w, &size.h) < 0) {
            throw std::runtime_error(TTF_GetError());
        }
        return size;
    }

    SDL_Rect TextRenderer::CalculateTextRect(const std::u16string &str) const
    {
        SDL_Rect size = MakeRect(GetTopLeftBoxPoint(), 0, 0);

        std::basic_string<Uint16> uint16Str = ToUint16String(str);
        
        if(TTF_SizeUNICODE(mCurrentFont, uint16Str.c_str(), &size.w, &size.h) < 0) {
            throw std::runtime_error(TTF_GetError());
        }
        return size;
    }
    
    void TextRenderer::LoadIdentity()
    {
        mCursorX = 0;
        mCursorY = 0;
    }
    
    void TextRenderer::Translate(int dx, int dy)
    {
        mCursorX += dx;
        mCursorY += dy;
    }

    void TextRenderer::SetColor(const SDL_Color &color)
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
    
    TTF_Font* TextRenderer::GetFont()
    {
        return mCurrentFont;
    }
} // namespace Render

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <memory>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include <game/sdl_utils.h>
#include <game/atlas.h>

namespace Render
{
    enum class CursorMode : int {
        TopLeft,
        BaseLine,
        BottomLeft
    };

    enum FontStyle
    {
        FontStyle_Normal = 0,
        FontStyle_Bold = 1,
        FontStyle_Italic = 2,
        FontStyle_Underline = 4,
        FontStyle_Strike = 8
    };
    
    class TextRenderer
    {
        Surface &mSurface;
        SDL_Color mColor {0, 0, 0, 0};
        int mCursorX = 0;
        int mCursorY = 0;
        CursorMode mCursorMode = CursorMode::BaseLine;
        TTF_Font *mCurrentFont = nullptr;
        SDL_Rect mClipBox {0, 0, 0, 0};
        int mFontStyle = FontStyle_Normal;

        SDL_Point GetTopLeftBoxPoint() const;
        void PutRenderedString(Surface &text);

        void CheckFontIsSet() const;

    public:
        TextRenderer(Surface &surface);
        TextRenderer(TextRenderer const&) = delete;
        TextRenderer& operator=(TextRenderer const&) = delete;

        virtual void PutString(const std::string &str);
        virtual void PutString(const std::u16string &str);

        void SetFontStyle(int style);
        void SetClipBox(const SDL_Rect &clipbox);
        void SetFont(TTF_Font *font);
        TTF_Font* GetFont();
        void SetColor(const SDL_Color &color);
        void SetCursorMode(CursorMode mode);
        void LoadIdentity();
        void Translate(int dx, int dy);
        
        SDL_Rect CalculateTextRect(const std::u16string &str) const;
        SDL_Rect CalculateTextRect(const std::string &str) const;
    };
    
} // namespace Renderer

#endif

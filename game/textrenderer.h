#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <memory>
#include <string>

#include <SDL_ttf.h>

#include <game/color.h>
#include <game/point.h>
#include <game/rect.h>
#include <game/sdl_utils.h>

class Surface;

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
        Color mColor;
        Point mCursor;
        CursorMode mCursorMode = CursorMode::BaseLine;
        TTF_Font *mCurrentFont = nullptr;
        Rect mClipBox;
        int mFontStyle = FontStyle_Normal;

        Point GetTopLeftBoxPoint() const;
        void PutRenderedString(Surface &text);

        void CheckFontIsSet() const;

    public:
        TextRenderer(Surface &surface);
        TextRenderer(TextRenderer const&) = delete;
        TextRenderer& operator=(TextRenderer const&) = delete;

        virtual void PutString(const std::string &str);
        virtual void PutString(const std::u16string &str);

        void SetFontStyle(int style);
        void SetClipBox(const Rect &clipbox);
        void SetFont(TTF_Font *font);
        TTF_Font* GetFont();
        void SetColor(const Color &color);
        void SetCursorMode(CursorMode mode);
        void LoadIdentity();
        void Translate(int dx, int dy);
        void SetCursorPos(Point pos);
        
        Rect CalculateTextRect(const std::u16string &str) const;
        Rect CalculateTextRect(const std::string &str) const;
    };
    
} // namespace Renderer

#endif

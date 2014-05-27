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

    enum TextAlignment
    {
        TextAlignment_Left = 1,
        TextAlignment_HCenter = 2,
        TextAlignment_Right = 4,
        TextAlignment_HJustified = 8,
        TextAlignment_VCenter = 16,
        TextAlignment_Up = 32,
        TextAlignment_Bottom = 64,
        TextAlignment_VJustified = 128
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
        TTF_Font *mCurrentFont;
        Rect mClipBox;
        int mFontStyle = FontStyle_Normal;
        int mFontAlignment;

        Point GetTopLeftBoxPoint() const;
        void PutRenderedString(Surface &text);
        void CheckFontIsSet() const;

    public:
        explicit TextRenderer(Surface &surface);
        TextRenderer(TextRenderer const&) = delete;
        TextRenderer& operator=(TextRenderer const&) = delete;

        void PutString(const std::string &str);

        void SetFontStyle(int style);
        void SetClipBox(const Rect &clipbox);
        void SetFont(TTF_Font *font);
        void SetColor(const Color &color);
        void SetCursorMode(CursorMode mode);
        void SetCursorPos(Point pos);
        
        TTF_Font* Font();
        Rect CalculateTextRect(const std::string &str) const;
    };
    
} // namespace Renderer

#endif

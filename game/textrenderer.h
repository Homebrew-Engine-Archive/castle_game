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
        FontStyle_Normal = TTF_STYLE_NORMAL,
        FontStyle_Bold = TTF_STYLE_BOLD,
        FontStyle_Italic = TTF_STYLE_ITALIC,
        FontStyle_Underline = TTF_STYLE_UNDERLINE,
        FontStyle_Strike = TTF_STYLE_STRIKETHROUGH
    };
    
    class TextRenderer
    {
        Surface &mSurface;
        Color mColor;
        Point mCursor;
        CursorMode mCursorMode = CursorMode::BaseLine;
        Rect mClipBox;
        int mFontStyle = FontStyle_Normal;
        int mFontAlignment = TextAlignment_Left | TextAlignment_Up;
        std::string mFontName;
        int mFontSize;
        TTF_Font *mCurrentFont;
        
        Point GetTopLeftBoxPoint() const;
        void PutRenderedString(Surface &text);

    public:
        explicit TextRenderer(Surface &surface);

        void PutString(const std::string &str);

        void SetFontStyle(int style);
        void SetClipBox(const Rect &clipbox);
        void SetFont(const std::string &fontname, int fsize);
        void SetColor(const Color &color);
        void SetCursorMode(CursorMode mode);
        void SetCursorPos(Point pos);

        const Point GetCursorPos() const;
        const std::string FontName() const;
        int FontSize() const;
        const Rect CalculateTextRect(const std::string &str) const;
    };
    
} // namespace Renderer

#endif

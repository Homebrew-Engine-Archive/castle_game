#ifndef TEXTAREA_H_
#define TEXTAREA_H_

#include <string>

#include <game/font.h>
#include <game/alignment.h>
#include <game/rect.h>
#include <game/color.h>
#include <game/textlayout.h>

namespace Render
{
    class Renderer;
}

namespace UI
{
    class TextArea
    {
    public:
        explicit TextArea();
        std::string const& Text() const;
        void SetText(const std::string &text);
        void AppendText(const std::string &text);
        void SetFont(const core::Font &font);
        const core::Font GetFont() const;
        void SetMaxWidth(int maxWidth);
        void SetMaxHeight(int maxHeight);
        void SetMinWidth(int minWidth);
        void SetMinHeight(int minHeight);
        void SetMultiline(bool ok);
        void SetAlignment(Alignment horizontal, Alignment vertical);
        void SetTextAlignment(Alignment horizontal);
        void SetTextColor(const Color &textColor);
        void SetBackgroundColor(const Color &backColor);
        const Rect FitToScreen(Render::Renderer &renderer) const;
        void Render(Render::Renderer &renderer);
        
    protected:
        mutable TextLayout mTextLayout;
        std::string mText;
        Rect mMaxSize;
        Rect mMinSize;
        Color mTextColor;
        Color mBackgroundColor;
        Alignment mHorizontalAlign;
        Alignment mVerticalAlign;
        core::Font mTextFont;
    };
}

#endif // TEXTAREA_H_

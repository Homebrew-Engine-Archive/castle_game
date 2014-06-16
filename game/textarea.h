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
        void SetAlignment(core::Alignment horizontal, core::Alignment vertical);
        void SetTextAlignment(core::Alignment horizontal);
        void SetTextColor(const core::Color &textColor);
        void SetBackgroundColor(const core::Color &backColor);
        const core::Rect FitToScreen(Render::Renderer &renderer) const;
        void Render(Render::Renderer &renderer);
        
    private:
        mutable TextLayout mTextLayout;
        std::string mText;
        core::Rect mMaxSize;
        core::Rect mMinSize;
        core::Color mTextColor;
        core::Color mBackgroundColor;
        core::Alignment mHorizontalAlign;
        core::Alignment mVerticalAlign;
        core::Font mTextFont;
    };
}

#endif // TEXTAREA_H_

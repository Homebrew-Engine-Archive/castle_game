#ifndef TEXTAREA_H_
#define TEXTAREA_H_

#include <memory>
#include <string>

#include <core/font.h>
#include <core/alignment.h>
#include <core/rect.h>
#include <core/color.h>
#include <core/size.h>

#include <game/textlayout.h>

namespace castle
{
    namespace render
    {
        class Renderer;
    }
    
    namespace ui
    {
        class TextArea
        {
        public:
            TextArea(TextArea const&) = delete;
            TextArea& operator=(TextArea const&) = delete;
            TextArea(TextArea&&) =default;
            TextArea& operator=(TextArea&&) = default;

            explicit TextArea();
            virtual ~TextArea() = default;
            
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
            const core::Rect FitToScreen(render::Renderer &renderer) const;
            void Render(render::Renderer &renderer);
        
        private:
            mutable std::unique_ptr<render::TextLayout> mTextLayout;
            std::string mText;
            core::Size mMaxSize;
            core::Size mMinSize;
            core::Color mTextColor;
            core::Color mBackgroundColor;
            core::Alignment mHorizontalAlign;
            core::Alignment mVerticalAlign;
            core::Font mTextFont;
        };
    }
}

#endif // TEXTAREA_H_

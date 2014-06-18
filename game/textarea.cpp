#include "textarea.h"

#include <iostream>

#include <game/textlayout.h>
#include <game/image.h>
#include <game/rect.h>
#include <game/fontengine.h>
#include <game/renderer.h>
#include <game/range.h>
#include <game/point.h>
#include <game/size.h>

namespace Castle
{
    namespace UI
    {
        TextArea::TextArea(TextArea const&) = delete;
        TextArea& TextArea::operator=(TextArea const&) = delete;
        TextArea::TextArea(TextArea&&) = default;
        TextArea& TextArea::operator=(TextArea&&) = default;
        TextArea::~TextArea() = default;
        
        TextArea::TextArea()
            : mTextLayout(new Render::TextLayout)
            , mText()
            , mMaxSize()
            , mMinSize()
            , mTextColor(core::colors::Black)
            , mBackgroundColor(core::colors::White)
            , mHorizontalAlign(core::Alignment::Min)
            , mVerticalAlign(core::Alignment::Min)
            , mTextFont()
        {
        }

        void TextArea::SetBackgroundColor(const core::Color &backgroundColor)
        {
            mBackgroundColor = backgroundColor;
        }

        void TextArea::SetTextColor(const core::Color &textColor)
        {
            mTextColor = textColor;
        }
    
        void TextArea::SetFont(const core::Font &font)
        {
            mTextFont = font;
            mTextLayout->SetFont(font);
        }

        const core::Font TextArea::GetFont() const
        {
            return mTextFont;
        }
    
        void TextArea::SetAlignment(core::Alignment horizontal, core::Alignment vertical)
        {
            mHorizontalAlign = horizontal;
            mVerticalAlign = vertical;
        }    

        void TextArea::SetTextAlignment(core::Alignment horizontal)
        {
            mTextLayout->SetAlignment(horizontal);
        }

        void TextArea::SetText(const std::string &text)
        {
            mText = text;
            mTextLayout->SetText(text);
        }

        void TextArea::AppendText(const std::string &text)
        {
            mText += text;
            mTextLayout->Insert(mText.size(), text);
        }
    
        std::string const& TextArea::Text() const
        {
            return mText;
        }
    
        void TextArea::SetMaxWidth(int maxWidth)
        {
            mMaxSize.w = maxWidth;
            mTextLayout->SetWidth(mMaxSize.w);
        }

        void TextArea::SetMaxHeight(int maxHeight)
        {
            mMaxSize.h = maxHeight;
        }
    
        void TextArea::SetMinWidth(int minWidth)
        {
            mMinSize.w = minWidth;
        }

        void TextArea::SetMinHeight(int minHeight)
        {
            mMinSize.h = minHeight;
        }
    
        const core::Rect TextArea::FitToScreen(Render::Renderer &renderer) const
        {
            mTextLayout->UpdateLayout(renderer.GetFontEngine());

            const core::Rect screenRect = renderer.GetScreenRect();
            const core::Rect textBoundingRect = mTextLayout->BoundingRect();
            const core::Rect textDrawRect = Union(mMinSize, textBoundingRect);
                    
            return core::CombineRect(
                AlignRange(AxisX(textDrawRect), AxisX(screenRect), mHorizontalAlign),
                AlignRange(AxisY(textDrawRect), AxisY(screenRect), mVerticalAlign));
        }
    
        void TextArea::Render(Render::Renderer &renderer)
        {
            const core::Rect widgetSubrect = FitToScreen(renderer);
            renderer.SetDrawColor(mBackgroundColor);
            renderer.FillFrame(widgetSubrect);
            renderer.SetViewport(widgetSubrect);

            renderer.BindFont(mTextFont);
            renderer.SetDrawColor(mTextColor);
            renderer.SetBackColor(mBackgroundColor);

            mTextLayout->UpdateLayout(renderer.GetFontEngine());

            const core::Size widgetSize = renderer.GetScreenSize();
            core::Rect drawArea = renderer.GetScreenRect();
            for(const Render::TextLayoutItem &item : *mTextLayout) {
                renderer.SetViewport(drawArea);

                const int offsetX = item.GetHorizontalOffset();
                const int offsetY = item.GetVerticalOffset();

                renderer.DrawText(item.GetItemText(), core::Point(offsetX, offsetY));

                const int advanceX = item.GetHorizontalAdvance();
                const int advanceY = item.GetVerticalAdvance();
                drawArea.x += advanceX;
                drawArea.y += advanceY;
                drawArea.w = std::max(0, widgetSize.width - drawArea.x);
                drawArea.h = std::max(0, widgetSize.height - drawArea.y);
                renderer.RestoreViewport();
            }

            renderer.RestoreViewport();
        }
    }
}

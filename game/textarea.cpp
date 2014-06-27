#include "textarea.h"

#include <iostream>

#include <game/textlayout.h>
#include <game/image.h>
#include <game/fontengine.h>
#include <game/renderer.h>

#include <core/interval.h>
#include <core/rect.h>
#include <core/point.h>
#include <core/size.h>

namespace castle
{
    namespace ui
    {
        TextArea::TextArea(TextArea const&) = delete;
        TextArea& TextArea::operator=(TextArea const&) = delete;
        TextArea::TextArea(TextArea&&) = default;
        TextArea& TextArea::operator=(TextArea&&) = default;
        TextArea::~TextArea() = default;
        
        TextArea::TextArea()
            : mTextLayout(new render::TextLayout)
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
            mMaxSize.SetWidth(maxWidth);
            mTextLayout->SetWidth(mMaxSize.Width());
        }

        void TextArea::SetMaxHeight(int maxHeight)
        {
            mMaxSize.SetHeight(maxHeight);
        }
    
        void TextArea::SetMinWidth(int minWidth)
        {
            mMinSize.SetWidth(minWidth);
        }

        void TextArea::SetMinHeight(int minHeight)
        {
            mMinSize.SetHeight(minHeight);
        }
    
        const core::Rect TextArea::FitToScreen(render::Renderer &renderer) const
        {
            mTextLayout->UpdateLayout(renderer.GetFontEngine());

            const core::Rect screenRect = renderer.GetScreenRect();
            const core::Rect textBoundingRect = mTextLayout->BoundingRect();
            const core::Rect textDrawRect = Union(core::Rect(mMinSize.Width(), mMinSize.Height()), textBoundingRect);
                    
            return core::CombineRect(
                core::AlignIntervals(AxisX(textDrawRect), AxisX(screenRect), mHorizontalAlign),
                core::AlignIntervals(AxisY(textDrawRect), AxisY(screenRect), mVerticalAlign));
        }
    
        void TextArea::Render(render::Renderer &renderer)
        {
            const core::Rect widgetSubrect = FitToScreen(renderer);
            renderer.SetDrawColor(mBackgroundColor);
            renderer.FillFrame(widgetSubrect);
            renderer.SetViewport(widgetSubrect);

            renderer.BindFont(mTextFont);
            renderer.SetDrawColor(mTextColor);
            renderer.SetBackColor(core::colors::Black.Opaque(0));

            mTextLayout->UpdateLayout(renderer.GetFontEngine());

            const core::Size widgetSize = renderer.GetScreenSize();
            core::Rect drawArea = renderer.GetScreenRect();
            for(const render::TextLayoutItem &item : *mTextLayout) {
                renderer.SetViewport(drawArea);

                const int offsetX = item.GetHorizontalOffset();
                const int offsetY = item.GetVerticalOffset();

                renderer.DrawText(item.GetItemText(), core::Point(offsetX, offsetY));

                const int advanceX = item.GetHorizontalAdvance();
                const int advanceY = item.GetVerticalAdvance();

                drawArea.SetX(drawArea.X() + advanceX);
                drawArea.SetY(drawArea.Y() + advanceY);
                drawArea.SetWidth(std::max<int>(0, widgetSize.Width() - drawArea.X()));
                drawArea.SetHeight(std::max<int>(0, widgetSize.Height() - drawArea.Y()));

                renderer.RestoreViewport();
            }

            renderer.RestoreViewport();
        }
    }
}

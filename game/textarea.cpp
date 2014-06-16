#include "textarea.h"

#include <iostream>

#include <game/surface.h>
#include <game/rect.h>
#include <game/fontmanager.h>
#include <game/renderer.h>
#include <game/range.h>

namespace UI
{
    TextArea::TextArea()
        : mTextLayout()
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
        mTextLayout.SetFont(font);
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
        mTextLayout.SetAlignment(horizontal);
    }

    void TextArea::SetText(const std::string &text)
    {
        mText = text;
        mTextLayout.SetText(text);
    }

    void TextArea::AppendText(const std::string &text)
    {
        mText += text;
        mTextLayout.Insert(mText.size(), text);
    }
    
    std::string const& TextArea::Text() const
    {
        return mText;
    }
    
    void TextArea::SetMaxWidth(int maxWidth)
    {
        mMaxSize.w = maxWidth;
        mTextLayout.SetWidth(mMaxSize.w);
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
        mTextLayout.UpdateLayout(renderer.GetFontManager());

        const core::Rect screenRect = renderer.GetScreenRect();
        const core::Rect textBoundingRect = mTextLayout.BoundingRect();
        const core::Rect textDrawRect = Union(mMinSize, textBoundingRect);
                    
        return core::CombineRect(
            AlignRange(AxisX(textDrawRect), AxisX(screenRect), mHorizontalAlign),
            AlignRange(AxisY(textDrawRect), AxisY(screenRect), mVerticalAlign));
    }
    
    void TextArea::Render(Render::Renderer &renderer)
    {
        const core::Rect widgetSubrect = FitToScreen(renderer);
        renderer.ClipRect(widgetSubrect);
        renderer.FillFrame(renderer.GetScreenRect(), mBackgroundColor);

        Render::FontManager &fontManager = renderer.GetFontManager();

        const core::Rect textAreaSize = renderer.GetScreenRect();
        core::Rect drawArea = textAreaSize;
        
        mTextLayout.UpdateLayout(fontManager);
        for(const TextLayoutItem &item : mTextLayout) {
            renderer.ClipRect(drawArea);
            
            const int offsetX = item.GetHorizontalOffset();
            const int offsetY = item.GetVerticalOffset();
            const core::Rect itemRect(
                offsetX,
                offsetY,
                drawArea.w - offsetX,
                drawArea.h - offsetY);

            renderer.ClipRect(itemRect);
            fontManager.DrawText(renderer, mTextFont, item.GetItemText(), mTextColor, mBackgroundColor);
            renderer.RestoreClipRect();

            const int advanceX = item.GetHorizontalAdvance();
            const int advanceY = item.GetVerticalAdvance();
            drawArea.x += advanceX;
            drawArea.y += advanceY;
            drawArea.w = std::max(0, textAreaSize.w - drawArea.x);
            drawArea.h = std::max(0, textAreaSize.h - drawArea.y);
            renderer.RestoreClipRect();
        }

        renderer.RestoreClipRect();
    }
}

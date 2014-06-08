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
        , mTextColor(Colors::Black)
        , mBackgroundColor(Colors::White)
        , mHorizontalAlign(Alignment::Min)
        , mVerticalAlign(Alignment::Min)
        , mTextFont()
    {
    }

    void TextArea::SetBackgroundColor(const Color &backgroundColor)
    {
        mBackgroundColor = backgroundColor;
    }

    void TextArea::SetTextColor(const Color &textColor)
    {
        mTextColor = textColor;
    }
    
    void TextArea::SetFont(const Font &font)
    {
        mTextFont = font;
        mTextLayout.SetFont(font);
    }

    Font TextArea::GetFont() const
    {
        return mTextFont;
    }
    
    void TextArea::SetAlignment(Alignment horizontal, Alignment vertical)
    {
        mHorizontalAlign = horizontal;
        mVerticalAlign = vertical;
    }    

    void TextArea::SetTextAlignment(Alignment horizontal, Alignment vertical)
    {
        mTextLayout.SetHorizontalAlignment(horizontal);
        mTextLayout.SetVerticalAlignment(vertical);
    }

    void TextArea::SetText(const std::string &text)
    {
        mText = text;
        mTextLayout.SetText(text);
    }

    void TextArea::AppendText(const std::string &text)
    {
        mText += text;
        mTextLayout.AppendText(text);
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
        mTextLayout.SetHeight(mMaxSize.w);
    }
    
    void TextArea::SetMinWidth(int minWidth)
    {
        mMinSize.w = minWidth;
    }

    void TextArea::SetMinHeight(int minHeight)
    {
        mMinSize.h = minHeight;
    }
    
    const Rect TextArea::FitToScreen(Render::Renderer &renderer) const
    {
        mTextLayout.UpdateLayout(renderer.GetFontManager());

        const Rect screenRect = renderer.GetScreenRect();
        const Rect textBoundingRect = mTextLayout.BoundingRect();
        const Rect textDrawRect = Union(mMinSize, textBoundingRect);
                    
        return CombineRect(
            AlignRange(AxisX(textDrawRect), AxisX(screenRect), mHorizontalAlign),
            AlignRange(AxisY(textDrawRect), AxisY(screenRect), mVerticalAlign));
    }
    
    void TextArea::Render(Render::Renderer &renderer)
    {
        const Rect widgetSubrect = FitToScreen(renderer);
        renderer.Clip(widgetSubrect);
        renderer.FillFrame(renderer.GetScreenRect(), mBackgroundColor);

        Render::FontManager &fontManager = renderer.GetFontManager();

        const Rect textAreaSize = renderer.GetScreenRect();
        Rect drawArea = textAreaSize;
        
        mTextLayout.UpdateLayout(fontManager);
        for(const TextLayoutItem &item : mTextLayout) {
            renderer.Clip(drawArea);
            
            const int offsetX = item.GetHorizontalOffset();
            const int offsetY = item.GetVerticalOffset();
            const Rect itemRect(
                offsetX,
                offsetY,
                drawArea.w - offsetX,
                drawArea.h - offsetY);

            renderer.Clip(itemRect);
            fontManager.DrawText(renderer, mTextFont, item.GetItemText(), mTextColor, mBackgroundColor);
            renderer.Unclip();

            const int advanceX = item.GetHorizontalAdvance();
            const int advanceY = item.GetVerticalAdvance();
            drawArea.x += advanceX;
            drawArea.y += advanceY;
            drawArea.w = std::max(0, textAreaSize.w - drawArea.x);
            drawArea.h = std::max(0, textAreaSize.h - drawArea.y);
            renderer.Unclip();
        }

        renderer.Unclip();
    }
}

#include "textlayoutitem.h"

namespace castle
{
    namespace render
    {
        TextLayoutItem::TextLayoutItem(std::string text, int advanceX, int advanceY, int offsetX, int offsetY)
            : mItemText(std::move(text))
            , mAdvanceX(advanceX)
            , mAdvanceY(advanceY)
            , mOffsetX(offsetX)
            , mOffsetY(offsetY)
        {}
        
        std::string const& TextLayoutItem::GetItemText() const
        {
            return mItemText;
        }

        int TextLayoutItem::GetHorizontalOffset() const
        {
            return mOffsetX;
        }
    
        int TextLayoutItem::GetVerticalOffset() const
        {
            return mOffsetY;
        }
    
        int TextLayoutItem::GetHorizontalAdvance() const
        {
            return mAdvanceX;
        }
    
        int TextLayoutItem::GetVerticalAdvance() const
        {
            return mAdvanceY;
        }

    }
}

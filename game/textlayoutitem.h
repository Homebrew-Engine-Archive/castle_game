#ifndef TEXTLAYOUTITEM_H_
#define TEXTLAYOUTITEM_H_

#include <string>

namespace castle
{
    namespace render
    {
        class TextLayoutItem
        {
        public:
            explicit TextLayoutItem(std::string text, int advanceX, int advanceY, int offsetX = 0, int offsetY = 0);
        
            std::string const& GetItemText() const;
            int GetHorizontalOffset() const;
            int GetVerticalOffset() const;
            int GetHorizontalAdvance() const;
            int GetVerticalAdvance() const;
        
        private:
            std::string mItemText;
            int mAdvanceX;
            int mAdvanceY;
            int mOffsetX;
            int mOffsetY;
        
        };
    }
}

#endif // TEXTLAYOUTITEM_H_

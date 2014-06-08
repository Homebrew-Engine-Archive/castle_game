#ifndef TEXTLAYOUT_H_
#define TEXTLAYOUT_H_

#include <exception>
#include <vector>
#include <string>

#include <game/rect.h>
#include <game/font.h>
#include <game/alignment.h>
#include <game/textlayoutitem.h>

namespace Render
{
    class FontManager;
}

namespace UI
{
    struct layout_error : std::exception
    {
        layout_error(std::string message) throw();
        const char* what() const throw();
    protected:
        const std::string mMessage;
    };
    
    class TextLayout
    {
    public:
        explicit TextLayout();

        void AppendText(const std::string &text);
        void RemoveText(size_t index);
        void SetText(const std::string &text);
        
        void SetFont(const UI::Font &font);
        void SetHorizontalAlignment(Alignment align);
        void SetVerticalAlignment(Alignment align);
        void SetMultiline(bool multiline);
        void SetWordWrap(bool ok);
        void SetWidth(int width);
        void SetHeight(int height);

        bool UpdateLayout(const Render::FontManager &fontManager);
        const Rect BoundingRect() const;
        std::vector<TextLayoutItem>::const_iterator begin() const
            {return mItems.begin();}
        
        std::vector<TextLayoutItem>::const_iterator end() const
            {return mItems.end();}

    protected:
        Alignment mHorizontalAlignment;
        Alignment mVerticalAlignment;
        bool mMultiline;
        int mWidth;
        int mHeight;
        bool mWordWrap;
        std::string mText;
        UI::Font mTextFont;
        std::vector<TextLayoutItem> mItems;
        Rect mBoundingRect;

        bool WasInvalidated() const;
        
    private:
        template<class Param>
        void SetParamAndInvalidate(Param &param, Param value) {
            if(param != value) {
                param = value;
                InvalidateLayout();
            }
        }

        void CommitLine(const Render::FontManager &engine, std::string line);
        void InvalidateLayout();
        void CalculateShrinkedLayout(const Render::FontManager &engine);
        void CalculateExpandedLayout(const Render::FontManager &engine);
    };
}

#endif // TEXTLAYOUT_H_

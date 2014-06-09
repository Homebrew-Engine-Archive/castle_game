#ifndef TEXTLAYOUT_H_
#define TEXTLAYOUT_H_

#include <iosfwd>
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

class TextLayoutLine;

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

        void Insert(size_t pos, const std::string &text);
        void Remove(size_t pos, size_t count = 1);
        void SetText(const std::string &text);
        
        void SetFont(const UI::Font &font);
        void SetAlignment(Alignment align);
        void SetWidth(int width);
        
        void InvalidateLayout();
        bool UpdateLayout(const Render::FontManager &fontManager);

        typedef std::vector<TextLayoutItem>::const_iterator const_iterator;
        
        const Rect BoundingRect() const;
        const_iterator begin() const;
        const_iterator end() const;

    private:        
        void ValidateLayout();        
        void CommitLine(const Render::FontManager &engine, std::string line);

        void PushSingleLine(const Render::FontManager &engine, const TextLayoutLine &line);
        void PushFirstLine(const Render::FontManager &engine, const TextLayoutLine &line);
        void PushLastLine(const Render::FontManager &engine, const TextLayoutLine &line);
        void PushLine(const Render::FontManager &engine, const TextLayoutLine &line);
        
        void PushShrinkedLine(const Render::FontManager &engine, const TextLayoutLine &line, int offset = 0);
        void PushExpandedLine(const Render::FontManager &engine, const TextLayoutLine &line);

        const TextLayoutLine GetLayoutLine(const Render::FontManager &engine, std::istream &in);
        
    protected:
        Alignment mHorizontalAlignment;
        int mWidth;
        std::string mText;
        UI::Font mTextFont;
        std::vector<TextLayoutItem> mItems;
        Rect mBoundingRect;
        bool mInvalidated;

    };
}

#endif // TEXTLAYOUT_H_

#ifndef TEXTLAYOUT_H_
#define TEXTLAYOUT_H_

#include <memory>
#include <iosfwd>
#include <exception>
#include <vector>
#include <string>

#include <game/rect.h>
#include <game/font.h>
#include <game/alignment.h>
#include <game/textlayoutitem.h>

namespace castle
{
    namespace render
    {
        class FontEngine;
    }

    namespace render
    {
        struct TextLayoutLine
        {
            std::string text;
        };
        
        struct layout_error : std::exception
        {
            layout_error(std::string message) throw();
            const char* what() const throw();
        private:
            const std::string mMessage;
        };
    
        class TextLayout
        {
        public:
            explicit TextLayout();
            TextLayout(TextLayout const&);
            TextLayout& operator=(TextLayout const&);
            TextLayout(TextLayout&&);
            TextLayout& operator=(TextLayout&&);
            virtual ~TextLayout();

            void Insert(size_t pos, const std::string &text);
            void Remove(size_t pos, size_t count = 1);
            void SetText(const std::string &text);
            void SetFont(const core::Font &font);
            void SetAlignment(const core::Alignment &alignment);
            void SetWidth(int width);
            void InvalidateLayout();
            bool UpdateLayout(const FontEngine &fontEngine);

            typedef std::vector<TextLayoutItem>::const_iterator const_iterator;
        
            const core::Rect BoundingRect() const;
            const_iterator begin() const;
            const_iterator end() const;

        private:        
            void ValidateLayout();        
            void CommitLine(const FontEngine &engine, std::string line);

            void PushSingleLine(const FontEngine &engine, const TextLayoutLine &line);
            void PushFirstLine(const FontEngine &engine, const TextLayoutLine &line);
            void PushLastLine(const FontEngine &engine, const TextLayoutLine &line);
            void PushLine(const FontEngine &engine, const TextLayoutLine &line);
        
            void PushShrinkedLine(const FontEngine &engine, const TextLayoutLine &line, int offset = 0);
            void PushExpandedLine(const FontEngine &engine, const TextLayoutLine &line);

            const TextLayoutLine GetLayoutLine(const FontEngine &engine, std::istream &in);
        
        protected:
            core::Alignment mHorizontalAlignment;
            int mWidth;
            std::string mText;
            core::Font mTextFont;
            std::vector<TextLayoutItem> mItems;
            core::Rect mBoundingRect;
            bool mInvalidated;

        };
    }
}

#endif // TEXTLAYOUT_H_

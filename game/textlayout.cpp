#include "textlayout.h"

/**
   to be done

   expanded horizontal alignment
   word wrap
   vertical alignment
   
**/

#include <functional>
#include <algorithm>
#include <iterator>
#include <locale>
#include <stdexcept>
#include <memory>
#include <sstream>

#include <game/size.h>
#include <game/fontengine.h>
#include <game/rect.h>
#include <game/range.h>

namespace Castle
{
    namespace Render
    {
        layout_error::layout_error(std::string message) throw()
            : mMessage(std::move(message))
        {
        }
    
        const char* layout_error::what() const throw()
        {
            return mMessage.c_str();
        }

        TextLayout::TextLayout(TextLayout const&) = default;
        TextLayout& TextLayout::operator=(TextLayout const&) = default;
        TextLayout::TextLayout(TextLayout&&) = default;
        TextLayout& TextLayout::operator=(TextLayout&&) = default;
        TextLayout::~TextLayout() = default;
        
        TextLayout::TextLayout()
            : mHorizontalAlignment(core::Alignment::Min)
            , mWidth(0)
            , mText()
            , mTextFont()
            , mItems()
            , mInvalidated(true)
        {
        }

        TextLayout::const_iterator TextLayout::begin() const
        {
            if(mInvalidated) {
                throw layout_error("layout is invalid");
            }
            return mItems.begin();
        }

        TextLayout::const_iterator TextLayout::end() const
        {
            return mItems.end();
        }

        template<class Iter>
        std::istream& ReadBlank(std::istream &in, Iter first)
        {
            while(in) {
                const auto character = in.get();
                if(!in) {
                    break;
                }
                if(!isblank(character)) {
                    break;
                }
                *first++ = character;
            }
            return in;
        }
    
        const TextLayoutLine TextLayout::GetLayoutLine(const FontEngine &engine, std::istream &in)
        {
            std::string buffer;
            while(in) {
                const auto character = in.get();
                if(!in) {
                    break;
                }
                if(character == '\n') {
                    break;
                }
                buffer += character;
                const core::Size bufferSize = engine.TextSize(mTextFont, buffer);
                if(bufferSize.width > mWidth) {
                    if(isblank(character)) {
                        in.unget();
                        buffer.pop_back();
                    } else {
                        while(!buffer.empty() && !isblank(buffer.back())) {
                            in.unget();
                            buffer.pop_back();
                        }
                        if(buffer.empty()) {
                            throw layout_error("layout width too small");
                        }
                    }
                    break;
                }
            }
        
            TextLayoutLine tmp;
            tmp.text = std::move(buffer);
            return tmp;
        }
    
        bool TextLayout::UpdateLayout(const FontEngine &engine)
        {
            if(mInvalidated) {
                mItems.clear();
                mBoundingRect = core::Rect(0, 0, 0, 0);

                std::vector<TextLayoutLine> lines;
                std::istringstream iss(mText);
                while(iss) {
                    const TextLayoutLine tmp = GetLayoutLine(engine, iss);
                    if(iss || !tmp.text.empty()) {
                        lines.push_back(tmp);
                    }
                }

                switch(lines.size()) {
                case 0:
                    break;
                
                case 1:
                    PushSingleLine(engine, lines[0]);
                    break;
                
                default:
                    {
                        PushFirstLine(engine, lines[0]);
                        for(size_t i = 1; i < lines.size() - 1; ++i) {
                            PushLine(engine, lines[i]);
                        }
                        PushLastLine(engine, lines.back());
                    }
                    break;
                }
            
                mInvalidated = false;
                return true;
            }

            return false;
        }
    
        void TextLayout::PushSingleLine(const FontEngine &engine, const TextLayoutLine &line)
        {
            if(mHorizontalAlignment == core::Alignment::Expanded) {
                return PushShrinkedLine(engine, line);
            } else {
                return PushLine(engine, line);
            }
        }
    
        void TextLayout::PushFirstLine(const FontEngine &engine, const TextLayoutLine &line)
        {
            PushLine(engine, line);
        }
    
        void TextLayout::PushLastLine(const FontEngine &engine, const TextLayoutLine &line)
        {
            PushSingleLine(engine, line);
        }
    
        void TextLayout::PushLine(const FontEngine &engine, const TextLayoutLine &line)
        {
            const core::Size lineSize = engine.TextSize(mTextFont, line.text);
            switch(mHorizontalAlignment) {
            case core::Alignment::Min:
                PushShrinkedLine(engine, line, 0);
                break;
            case core::Alignment::Max:
                PushShrinkedLine(engine, line, (mWidth - lineSize.width));
                break;
            case core::Alignment::Center:
                PushShrinkedLine(engine, line, (mWidth - lineSize.width) / 2);
                break;
            case core::Alignment::Expanded:
                PushExpandedLine(engine, line);
                break;
            default:
                throw std::runtime_error("wrong alignment");
            }
            mBoundingRect.w = std::max<int>(mBoundingRect.w, lineSize.width);
        }
    
        void TextLayout::PushShrinkedLine(const FontEngine &engine, const TextLayoutLine &line, int offset)
        {
            const int lineSkip = engine.LineSkip(mTextFont);
            mBoundingRect.h += lineSkip;
            mItems.emplace_back(line.text, 0, lineSkip, offset, 0);
        }
    
        void TextLayout::PushExpandedLine(const FontEngine &engine, const TextLayoutLine &line)
        {
            PushShrinkedLine(engine, line, 0);
        }
    
        void TextLayout::SetText(const std::string &text)
        {
            if(mText != text) {
                mText = text;
                InvalidateLayout();
            }
        }

        void TextLayout::Insert(size_t pos, const std::string &text)
        {
            if(!text.empty()) {
                mText.insert(pos, text);
                InvalidateLayout();
            }
        }

        void TextLayout::Remove(size_t index, size_t count)
        {
            if(count != 0) {
                mText.erase(index, count);
                InvalidateLayout();
            }
        }
    
        void TextLayout::SetFont(const core::Font &font)
        {
            if(mTextFont != font) {
                mTextFont = font;
                InvalidateLayout();
            }
        }

        void TextLayout::SetAlignment(core::Alignment horizontal)
        {
            if(mHorizontalAlignment != horizontal) {
                mHorizontalAlignment = horizontal;
                InvalidateLayout();
            }
        }

        void TextLayout::SetWidth(int width)
        {
            if(mWidth != width) {
                mWidth = width;
                InvalidateLayout();
            }
        }

        const core::Rect TextLayout::BoundingRect() const
        {
            return mBoundingRect;
        }
    
        void TextLayout::InvalidateLayout()
        {
            mInvalidated = true;
        }

        void TextLayout::ValidateLayout()
        {
            mInvalidated = false;
        }
    } // namespace UI
}

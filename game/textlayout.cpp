#include "textlayout.h"

#include <memory>
#include <sstream>

#include <game/fontmanager.h>
#include <game/rect.h>

namespace UI
{
    layout_error::layout_error(std::string message) throw()
        : mMessage(std::move(message))
    {
    }
    
    const char* layout_error::what() const throw()
    {
        return mMessage.c_str();
    }
}

namespace UI
{    
    TextLayout::TextLayout()
        : mHorizontalAlignment(Alignment::Min)
        , mVerticalAlignment(Alignment::Min)
        , mMultiline(true)
        , mWidth(0)
        , mHeight(0)
        , mWordWrap(false)
        , mText()
        , mTextFont()
        , mItems()
    {
    }

    void TextLayout::SetText(const std::string &text)
    {
        SetParamAndInvalidate(mText, text);
    }

    void TextLayout::AppendText(const std::string &text)
    {
        const std::string newText = mText + text;
        SetParamAndInvalidate(mText, text);
    }

    void TextLayout::RemoveText(size_t text)
    {
        
    }
    
    void TextLayout::SetFont(const UI::Font &font)
    {
        SetParamAndInvalidate(mTextFont, font);
    }

    void TextLayout::SetWordWrap(bool wrap)
    {
        SetParamAndInvalidate(mWordWrap, wrap);
    }

    void TextLayout::SetMultiline(bool multiline)
    {
        SetParamAndInvalidate(mMultiline, multiline);
    }

    void TextLayout::SetVerticalAlignment(Alignment vertical)
    {
        SetParamAndInvalidate(mVerticalAlignment, vertical);
    }

    void TextLayout::SetHorizontalAlignment(Alignment horizontal)
    {
        SetParamAndInvalidate(mHorizontalAlignment, horizontal);
    }

    void TextLayout::SetWidth(int width)
    {
        SetParamAndInvalidate(mWidth, width);
    }
    
    void TextLayout::SetHeight(int height)
    {
        SetParamAndInvalidate(mHeight, height);
    }
    
    const Rect TextLayout::BoundingRect() const
    {
        return mBoundingRect;
    }
    
    void TextLayout::InvalidateLayout()
    {
        mItems.clear();
        mBoundingRect = Rect(0, 0, 0, 0);
    }

    void TextLayout::CommitLine(const Render::FontManager &engine, std::string block)
    {
        const int lineSkip = engine.LineSkip(mTextFont);
        const Rect blockSize = engine.TextSize(mTextFont, block);
        mBoundingRect.w = std::max(mBoundingRect.w, blockSize.w);
        mBoundingRect.h += lineSkip;

        switch(mHorizontalAlignment) {
        case Alignment::Min:
            mItems.emplace_back(std::move(block), 0, lineSkip, 0, 0);
            break;
            
        case Alignment::Max:
            mItems.emplace_back(std::move(block), 0, lineSkip, mWidth - blockSize.w, 0);
            break;

        case Alignment::Expanded:
            mItems.emplace_back(std::move(block), 0, lineSkip);
            break;
            
        case Alignment::Center:
            mItems.emplace_back(std::move(block), 0, lineSkip, (mWidth - blockSize.w) / 2, 0);
            break;
        
            // case Alignment::Expanded:
            break;
        }
    }
    
    bool TextLayout::UpdateLayout(const Render::FontManager &engine)
    {
        if(WasInvalidated()) {
            switch(mHorizontalAlignment) {
            case Alignment::Min:
            case Alignment::Max:
            case Alignment::Center:
                CalculateShrinkedLayout(engine);
                break;
            case Alignment::Expanded:
                CalculateExpandedLayout(engine);
                break;
            }
            return true;
        }

        return false;
    }

    struct CharBlock
    {
        bool Append(char character)
            {return true;}
    };

    struct WordBlock
    {
        bool Append(std::string word)
            {return true;}
    };
    
    void TextLayout::CalculateShrinkedLayout(const Render::FontManager &engine)
    {
        std::istringstream iss(mText);
        std::string buffer;
        
        while(iss) {
            const auto character = iss.get();
            if(!iss) {
                break;
            }
                
            switch(character) {
            case '\r':
                break;
            case '\n':
                CommitLine(engine, std::move(buffer));
                break;
            case '\t':
            case ' ':
            default:
                buffer += character;
            }
                
            const Rect bufferSize = engine.TextSize(mTextFont, buffer);
            if((mWidth > 0) && (bufferSize.w > mWidth)) {
                buffer.pop_back();
                iss.unget();
                if(buffer.empty()) {
                    throw layout_error("width too small - can't even place one single char");
                }
                CommitLine(engine, std::move(buffer));
            }
        }

        if(!buffer.empty()) {
            CommitLine(engine, std::move(buffer));
        }
    }

    void TextLayout::CalculateExpandedLayout(const Render::FontManager &engine)
    {
        CalculateShrinkedLayout(engine);
    }
    
    bool TextLayout::WasInvalidated() const
    {
        return !mText.empty() && mItems.empty();
    }
}

#include "font.h"

#include <iostream>

namespace UI
{
    Font::Font()
        : Font(std::string(), 16)
    {}
    
    Font::Font(const std::string &family, int height)
        : mFontFamily(family)
        , mFontHeight(height)
        , mFontAntialiased(true)
        , mFontHinted(true)
        , mFontOutline(0)
        , mFontKerning(1)
        , mStyleBold(false)
        , mStyleItalic(false)
        , mStyleStrikethrough(false)
        , mStyleUnderline(false)
    {
    }
    
    const std::string Font::Family() const
    {
        return mFontFamily;
    }
    
    int Font::Height() const
    {
        return mFontHeight;
    }
    
    bool Font::Antialiased() const
    {
        return mFontAntialiased;
    }

    bool Font::Hinted() const
    {
        return mFontHinted;
    }
    
    int Font::Outline() const
    {
        return mFontOutline;
    }
    
    int Font::Kerning() const
    {
        return mFontKerning;
    }
    
    bool Font::Bold() const
    {
        return mStyleBold;
    }
    
    bool Font::Italic() const
    {
        return mStyleItalic;
    }
    
    bool Font::Strikethrough() const
    {
        return mStyleStrikethrough;
    }
    
    bool Font::Underline() const
    {
        return mStyleUnderline;
    }
    
    Font& Font::Family(const std::string &family)
    {
        mFontFamily = family;
        return *this;
    }
    
    Font& Font::Height(int height)
    {
        mFontHeight = height;
        return *this;
    }
    
    Font& Font::Antialiased(bool antialiased)
    {
        mFontAntialiased = antialiased;
        return *this;
    }
    
    Font& Font::Hinted(bool hinted)
    {
        mFontHinted = hinted;
        return *this;
    }
    
    Font& Font::Outline(int outline)
    {
        mFontOutline = outline;
        return *this;
    }
    
    Font& Font::Kerning(int kerning)
    {
        mFontKerning = kerning;
        return *this;
    }
    
    Font& Font::Bold(bool bold)
    {
        mStyleBold = bold;
        return *this;
    }
    
    Font& Font::Italic(bool italic)
    {
        mStyleItalic = italic;
        return *this;
    }
    
    Font& Font::Strikethrough(bool strikethrough)
    {
        mStyleStrikethrough = strikethrough;
        return *this;
    }
    
    Font& Font::Underline(bool underline)
    {
        mStyleUnderline = underline;
        return *this;
    }

    bool IsCopyOf(const Font &lhs, const Font &rhs)
    {
        return (lhs.Family() == rhs.Family()) && (lhs.Height() == rhs.Height());
    }

    bool operator==(const Font &lhs, const Font &rhs)
    {
        return lhs.Family() == rhs.Family()
            && lhs.Height() == rhs.Height()
            && lhs.Antialiased() == rhs.Antialiased()
            && lhs.Hinted() == rhs.Hinted()
            && lhs.Outline() == rhs.Outline()
            && lhs.Kerning() == rhs.Kerning()
            && lhs.Bold() == rhs.Bold()
            && lhs.Italic() == rhs.Italic()
            && lhs.Strikethrough() == rhs.Strikethrough()
            && lhs.Underline() == rhs.Underline();
    }

    bool operator!=(const Font &lhs, const Font &rhs)
    {
        return !(lhs == rhs);
    }
    
    std::ostream& operator<<(std::ostream &out, const Font &font)
    {
        out << font.Family() << ' ' << font.Height();
        return out;
    }
} // namespace UI

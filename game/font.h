#ifndef FONT_H_
#define FONT_H_

#include <iosfwd>
#include <string>

namespace core
{
    class Font
    {        
    public:
        Font();
        explicit Font(const std::string &family, int height);
        Font(Font&&);                                    // = default
        Font(Font const&);                               // = default
        Font& operator=(Font&&);                         // = default
        Font& operator=(Font const&);                    // = default
        virtual ~Font();                                 // = default

        const std::string Family() const;
        int Height() const;
        bool Antialiased() const;
        bool Hinted() const;
        int Outline() const;
        int Kerning() const;
        bool Bold() const;
        bool Italic() const;
        bool Strikethrough() const;
        bool Underline() const;

        Font& Family(const std::string &family);
        Font& Height(int height);
        Font& Antialiased(bool antialiased);
        Font& Hinted(bool hinted);
        Font& Outline(int outline);
        Font& Kerning(int kerning);
        Font& Bold(bool bold);
        Font& Italic(bool italic);
        Font& Strikethrough(bool strikethrough);
        Font& Underline(bool underline);
        
    private:
        std::string mFontFamily;
        int mFontHeight;
        bool mFontAntialiased;
        bool mFontHinted;
        int mFontOutline;
        int mFontKerning;
        bool mStyleBold;
        bool mStyleItalic;
        bool mStyleStrikethrough;
        bool mStyleUnderline;

    };

    bool IsCopyOf(const Font &lhs, const Font &rhs);
    bool operator==(const Font &lhs, const Font &rhs);
    bool operator!=(const Font &lhs, const Font &rhs);
    
    std::ostream& operator<<(std::ostream &out, const Font &font);
}

#endif // FONT_H_

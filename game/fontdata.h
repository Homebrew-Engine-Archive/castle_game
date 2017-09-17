#ifndef FONTDATA_H_
#define FONTDATA_H_

#include <memory>

#include <core/size.h>
#include <core/font.h>
#include <core/image.h>

#include <game/ttf_init.h>

struct FontCloseDeleter
{
    void operator()(TTF_Font *font)
        {
            TTF_CloseFont(font);
        }
};

typedef std::unique_ptr<TTF_Font, FontCloseDeleter> TTFFontPtr;

class FontData
{
public:
    FontData(const core::Font &font, TTFFontPtr ttfFont);
    FontData(FontData&&) = default;
    FontData(const FontData &that) = delete;
    FontData& operator=(const FontData &that) = delete;
    virtual ~FontData() = default;

    core::Font const& Font() const;
    /**
       Produces as result argb32 surface considered to be drawn on screen.
    **/
    core::Image RenderBlended(const std::string &text, const core::Color &fg) const;

    /**
       Palettized surface with background
    **/
    core::Image RenderShaded(const std::string &text, const core::Color &fg, const core::Color &bg) const;

    /**
       Cheap and fast
    **/
    core::Image RenderSolid(const std::string &text, const core::Color &fg) const;
    
    bool HasGlyph(int character) const;
    const core::Size TextSize(const std::string &text) const;
    void UpdateFontState(const core::Font &font) const;
    int LineSkip() const;
    
protected:
    core::Font mFont;

    /**
       UpdateFontState actually changes this object.
    **/
    mutable TTFFontPtr mFontObject;
};

#endif
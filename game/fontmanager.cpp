#include "fontmanager.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <game/gm1palette.h>
#include <game/font.h>
#include <game/renderer.h>
#include <game/surface.h>
#include <game/rect.h>
#include <game/color.h>
#include <game/ttf_error.h>
#include <game/sdl_utils.h>
#include <game/ttf_utils.h>

namespace
{
    struct font_error : public virtual std::runtime_error
    {
        font_error()
            : std::runtime_error("font lookup failed")
            {}
    };
    
    int GetFontHinting(const core::Font &font)
    {
        return (font.Hinted() ? TTF_HINTING_NORMAL : TTF_HINTING_NONE);
    }

    int GetFontStyle(const core::Font &font)
    {
        return (font.Bold() ? TTF_STYLE_BOLD : 0)
            | (font.Italic() ? TTF_STYLE_ITALIC : 0)
            | (font.Underline() ? TTF_STYLE_UNDERLINE : 0)
            | (font.Strikethrough() ? TTF_STYLE_STRIKETHROUGH : 0);
    }
}

class FontData
{
public:
    FontData(const core::Font &font, FontPtr ttfFont)
        : mFont(font)
        , mFontObject(std::move(ttfFont))
        {}
    
    FontData(FontData&&) = default;
    FontData(const FontData &that) = delete;
    FontData& operator=(const FontData &that) = delete;
    ~FontData() = default;

    core::Font const& Font() const;
    /**
       Produces as result argb32 surface considered to be drawn on screen.
     **/
    Surface RenderBlended(const std::string &text, const Color &fg) const;

    /**
       Palettized surface with background
     **/
    Surface RenderShaded(const std::string &text, const Color &fg, const Color &bg) const;

    /**
       Cheap and fast
     **/
    Surface RenderSolid(const std::string &text, const Color &fg) const;
    
    bool HasGlyph(int character) const;
    const Rect TextSize(const std::string &text) const;
    void UpdateFontState(const core::Font &font) const;
    int LineSkip() const;
    
protected:
    core::Font mFont;

    /**
       UpdateFontState actually changes this object.
     **/
    mutable FontPtr mFontObject;
};

const Rect FontData::TextSize(const std::string &text) const
{
    int width;
    int height;
    if(TTF_SizeText(mFontObject.get(), text.c_str(), &width, &height) < 0) {
        throw ttf_error();
    }
    return Rect(width, height);
}

const core::Font& FontData::Font() const
{
    return mFont;
}

void FontData::UpdateFontState(const core::Font &font) const
{
    TTF_Font *ttf_font = mFontObject.get();
    TTF_SetFontStyle(ttf_font, GetFontStyle(font));
    TTF_SetFontHinting(ttf_font, GetFontHinting(font));
    TTF_SetFontOutline(ttf_font, font.Outline());
    TTF_SetFontKerning(ttf_font, font.Kerning());
}

Surface FontData::RenderBlended(const std::string &text, const Color &fg) const
{
    return Surface(TTF_RenderUTF8_Blended(mFontObject.get(), text.c_str(), fg));
}

Surface FontData::RenderShaded(const std::string &text, const Color &fg, const Color &bg) const
{
    return Surface(TTF_RenderUTF8_Shaded(mFontObject.get(), text.c_str(), fg, bg));
}

Surface FontData::RenderSolid(const std::string &text, const Color &fg) const
{
    return Surface(TTF_RenderUTF8_Solid(mFontObject.get(), text.c_str(), fg));
}

bool FontData::HasGlyph(int character) const
{
    return (TTF_GlyphIsProvided(mFontObject.get(), character) == 0);
}

int FontData::LineSkip() const
{
    return TTF_FontLineSkip(mFontObject.get());
}

const FontData* GetBestMatch(const core::Font &font, const FontData *lhs, const FontData *rhs)
{
    if(lhs == nullptr) {
        return rhs;
    }
        
    if(rhs == nullptr) {
        return lhs;
    }

    const core::Font &alice = lhs->Font();
    const core::Font &bob = rhs->Font();

    if((alice.Family() == font.Family()) && (bob.Family() != font.Family())) {
        return lhs;
    }
        
    if((alice.Family() != font.Family()) && (bob.Family() == font.Family())) {
        return rhs;
    }

    if(abs(alice.Height() - font.Height()) < abs(bob.Height() - font.Height())) {
        return lhs;
    } else {
        return rhs;
    }
}

namespace Render
{
    FontManager::FontManager(FontManager const&) = delete;
    FontManager& FontManager::operator=(FontManager const&) = delete;
    FontManager::FontManager(FontManager&&) = default;
    FontManager& FontManager::operator=(FontManager&&) = delete;
    FontManager::~FontManager() = default;
    
    FontManager::FontManager()
        : mTTF_Init(new TTFInitializer)
        , mFontTable()
    {
    }    
    
    FontData const* FontManager::LookupFont(const core::Font &font) const
    {
        const FontData *result = nullptr;
        for(const FontData &fd : mFontTable) {
            result = GetBestMatch(font, result, &fd);
        }
        return result;
    }

    std::vector<fs::path> FontManager::FontSearchPathsList(const core::Font &font) const
    {
        std::vector<fs::path> paths;

        paths.push_back(fs::FontFilePath(font.Family()));
        paths.push_back(fs::FontFilePath(
                            boost::to_upper_copy(font.Family())));
        paths.push_back(fs::FontFilePath(
                            boost::to_lower_copy(font.Family())));
        
        return paths;
    }

    FontData FontManager::LoadFontData(const fs::path &path, const core::Font &font) const
    {
        /**
           TODO windows sucks with utf8 so much
           see http://stackoverflow.com/questions/11352641/boostfilesystempath-and-fopen
           so we must not use c_str().
                
           The desired solution is as follows
                
           boost::filesystem::ifstream fin(fontData.fontPath);
           RWPtr rw(SDL_RWFromInputStream(fin));
           TTF_Font *font = TTF_OpenFontRW(rw.get(), SDL_FALSE, fsize);
                
           ... but it gets a crash due to a bug in SDL_OpenFontRW
           see http://forums.libsdl.org/viewtopic.php?t=8050&sid=ba3720be045e8acadf2645d7369156f8
        **/
        const char *c_fpath = path.string().c_str();

        FontPtr ttf_font(TTF_OpenFont(c_fpath, font.Height()));
        if(!ttf_font) {
            throw ttf_error();
        }

        return FontData(font, std::move(ttf_font));
    }
    
    bool FontManager::LoadFont(const core::Font &font)
    {
        if(HasExactMatch(font)) {
            std::clog << "Font is already loaded: " << font << std::endl;
            return false;
        }

        std::vector<fs::path> paths = FontSearchPathsList(font);
        for(const fs::path &path : paths) {
            try {
                FontData temp = LoadFontData(path, font);
                AddFontData(std::move(temp));
                return true;
            } catch(const ttf_error &error) {
                std::clog << "Search font (" << font << ") in " << path << " failed: " << error.what() << std::endl;
            }
        }

        return false;
    }

    bool FontManager::HasFontLoaded(const core::Font &font) const
    {
        return !mFontTable.empty();
    }
    
    bool FontManager::HasExactMatch(const core::Font &font) const
    {
        const FontData *data = LookupFont(font);
        if(data == nullptr) {
            return false;
        }
        return IsCopyOf(data->Font(), font);
    }
    
    bool FontManager::CouldRender(const core::Font &font, const std::string &text) const
    {
        const FontData *fontData = LookupFont(font);
        if(fontData == nullptr) {
            return false;
        }
        fontData->UpdateFontState(font);
        for(auto character : text) {
            if(!fontData->HasGlyph(character)) {
                return false;
            }
        }
        return true;
    }

    void FontManager::AddFontData(FontData fontdata)
    {
        mFontTable.push_back(std::move(fontdata));
    }
    
    void FontManager::DrawText(Renderer &renderer, const core::Font &font, const std::string &text, const Color &fg, const Color &bg) const
    {
        if(!text.empty()) {
            const FontData *fontData = LookupFont(font);
            if(fontData == nullptr) {
                throw font_error();
            }
            
            fontData->UpdateFontState(font);
            Surface textSurface;
            if(bg.a == 0xff) {
                textSurface = fontData->RenderShaded(text.c_str(), fg, bg);
            } else if(font.Antialiased()) {
                textSurface = fontData->RenderBlended(text.c_str(), fg);
            } else {
                textSurface = fontData->RenderSolid(text.c_str(), fg);
            }
            if(!textSurface.Null()) {
                // \todo avoid conversion
                if(SDL_ISPIXELFORMAT_INDEXED(SurfaceFormat(textSurface).format)) {
                    textSurface = ConvertSurface(textSurface, SDL_PIXELFORMAT_ARGB8888);
                }
                renderer.Opacity(fg.a);
                renderer.BindSurface(textSurface);
                renderer.Blit(Rect(textSurface), Rect(textSurface));
                renderer.RestoreOpacity();
            } else {
                throw ttf_error();
            }
        }
    }

    const Rect FontManager::TextSize(const core::Font &font, const std::string &text) const
    {
        const FontData *fontData = LookupFont(font);
        if(fontData == nullptr) {
            throw font_error();
        }
        fontData->UpdateFontState(font);
        return fontData->TextSize(text);
    }

    int FontManager::LineSkip(const core::Font &font) const
    {
        const FontData *fontData = LookupFont(font);
        if(fontData == nullptr) {
            throw font_error();
        }
        fontData->UpdateFontState(font);
        return fontData->LineSkip();
    }
}

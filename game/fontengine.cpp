#include "fontengine.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <game/size.h>
#include <game/gm1palette.h>
#include <game/font.h>
#include <game/renderer.h>
#include <game/image.h>
#include <game/rect.h>
#include <game/color.h>
#include <game/ttf_error.h>
#include <game/sdl_utils.h>
#include <game/ttf_utils.h>
#include <game/point.h>

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
    Castle::Image RenderBlended(const std::string &text, const core::Color &fg) const;

    /**
       Palettized surface with background
    **/
    Castle::Image RenderShaded(const std::string &text, const core::Color &fg, const core::Color &bg) const;

    /**
       Cheap and fast
    **/
    Castle::Image RenderSolid(const std::string &text, const core::Color &fg) const;
    
    bool HasGlyph(int character) const;
    const core::Size TextSize(const std::string &text) const;
    void UpdateFontState(const core::Font &font) const;
    int LineSkip() const;
    
protected:
    core::Font mFont;

    /**
       UpdateFontState actually changes this object.
    **/
    mutable FontPtr mFontObject;
};

const core::Size FontData::TextSize(const std::string &text) const
{
    int width;
    int height;
    if(TTF_SizeText(mFontObject.get(), text.c_str(), &width, &height) < 0) {
        throw ttf_error();
    }
    return core::Size(width, height);
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

Castle::Image FontData::RenderBlended(const std::string &text, const core::Color &fg) const
{
    return Castle::Image(TTF_RenderUTF8_Blended(mFontObject.get(), text.c_str(), fg));
}

Castle::Image FontData::RenderShaded(const std::string &text, const core::Color &fg, const core::Color &bg) const
{
    return Castle::Image(TTF_RenderUTF8_Shaded(mFontObject.get(), text.c_str(), fg, bg));
}

Castle::Image FontData::RenderSolid(const std::string &text, const core::Color &fg) const
{
    return Castle::Image(TTF_RenderUTF8_Solid(mFontObject.get(), text.c_str(), fg));
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

namespace Castle
{
    namespace Render
    {
        FontEngine::FontEngine(FontEngine const&) = delete;
        FontEngine& FontEngine::operator=(FontEngine const&) = delete;
        FontEngine::FontEngine(FontEngine&&) = default;
        FontEngine& FontEngine::operator=(FontEngine&&) = delete;
        FontEngine::~FontEngine() = default;
    
        FontEngine::FontEngine()
            : mTTF_Init(new TTFInitializer)
            , mFontTable()
        {
        }    
    
        FontData const* FontEngine::LookupFont(const core::Font &font) const
        {
            const FontData *result = nullptr;
            for(const FontData &fd : mFontTable) {
                result = GetBestMatch(font, result, &fd);
            }
            return result;
        }

        std::vector<fs::path> FontEngine::FontSearchPathsList(const core::Font &font) const
        {
            std::vector<fs::path> paths;

            paths.push_back(fs::FontFilePath(font.Family()));
            paths.push_back(fs::FontFilePath(
                                boost::to_upper_copy(font.Family())));
            paths.push_back(fs::FontFilePath(
                                boost::to_lower_copy(font.Family())));
        
            return paths;
        }

        FontData FontEngine::LoadFontData(const fs::path &path, const core::Font &font) const
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
    
        bool FontEngine::LoadFont(const core::Font &font)
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

        bool FontEngine::HasFontLoaded(const core::Font &font) const
        {
            return !mFontTable.empty();
        }
    
        bool FontEngine::HasExactMatch(const core::Font &font) const
        {
            const FontData *data = LookupFont(font);
            if(data == nullptr) {
                return false;
            }
            return IsCopyOf(data->Font(), font);
        }
    
        bool FontEngine::CouldRender(const core::Font &font, const std::string &text) const
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

        void FontEngine::AddFontData(FontData fontdata)
        {
            mFontTable.push_back(std::move(fontdata));
        }
    
        void FontEngine::DrawText(Renderer &renderer, const core::Point &target, const core::Font &font, const std::string &text, const core::Color &fg, const core::Color &bg) const
        {
            if(!text.empty()) {
                const FontData *fontData = LookupFont(font);
                if(fontData == nullptr) {
                    throw font_error();
                }
            
                fontData->UpdateFontState(font);
                Image textImage;
                if(bg.a == 0xff) {
                    textImage = fontData->RenderShaded(text.c_str(), fg, bg);
                } else if(font.Antialiased()) {
                    textImage = fontData->RenderBlended(text.c_str(), fg);
                } else {
                    textImage = fontData->RenderSolid(text.c_str(), fg);
                }
                if(!textImage.Null()) {
                    // \todo avoid conversion
                    if(IsPalettized(textImage)) {
                        textImage = ConvertImage(textImage, SDL_PIXELFORMAT_ARGB8888);
                    }
                    renderer.Opacity(fg.a);
                    renderer.BindImage(textImage);
                    renderer.Blit(core::Rect(textImage), target);
                    renderer.RestoreOpacity();
                } else {
                    throw ttf_error();
                }
            }
        }

        const core::Size FontEngine::TextSize(const core::Font &font, const std::string &text) const
        {
            const FontData *fontData = LookupFont(font);
            if(fontData == nullptr) {
                throw font_error();
            }
            fontData->UpdateFontState(font);
            return fontData->TextSize(text);
        }

        int FontEngine::LineSkip(const core::Font &font) const
        {
            const FontData *fontData = LookupFont(font);
            if(fontData == nullptr) {
                throw font_error();
            }
            fontData->UpdateFontState(font);
            return fontData->LineSkip();
        }
    } // namespace Render
}

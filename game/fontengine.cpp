#include "fontengine.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/filesystem/fstream.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <core/size.h>
#include <core/font.h>
#include <core/rect.h>
#include <core/color.h>
#include <core/point.h>
#include <core/image.h>
#include <core/palette.h>

#include <game/renderengine.h>
#include <game/ttf_error.h>
#include <game/ttf_init.h>
#include <core/sdl_utils.h>

namespace
{    
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

const core::Size FontData::TextSize(const std::string &text) const
{
    int width;
    int height;
    if(TTF_SizeText(mFontObject.get(), text.c_str(), &width, &height) < 0) {
        throw ttf_error();
    }
    return core::Size(width, height);
}

FontData::FontData(const core::Font &font, TTFFontPtr fontObject)
    : mFont(font)
    , mFontObject(std::move(fontObject))
{
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

core::Image FontData::RenderBlended(const std::string &text, const core::Color &fg) const
{
    return core::Image(TTF_RenderUTF8_Blended(mFontObject.get(), text.c_str(), fg));
}

core::Image FontData::RenderShaded(const std::string &text, const core::Color &fg, const core::Color &bg) const
{
    return core::Image(TTF_RenderUTF8_Shaded(mFontObject.get(), text.c_str(), fg, bg));
}

core::Image FontData::RenderSolid(const std::string &text, const core::Color &fg) const
{
    return core::Image(TTF_RenderUTF8_Solid(mFontObject.get(), text.c_str(), fg));
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

namespace castle
{
    namespace render
    {
        const char* font_error::what() const throw()
        {
            return "font operation failed";
        }
    
        FontEngine::FontEngine()
            : mTTF_Init(new TTFInitializer())
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

        const FontData& FontEngine::GetFontContext(const core::Font &font) const
        {
            const FontData *fontData = LookupFont(font);
            if(fontData == nullptr) {
                throw font_error();
            }
            fontData->UpdateFontState(font);
            return *fontData;
        }
        
        FontData FontEngine::LoadFontData(const vfs::path &path, const core::Font &font) const
        {
            const char *c_fpath = path.string().c_str();

            TTFFontPtr ttf_font(TTF_OpenFont(c_fpath, font.Height()));
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

            const std::vector<vfs::path> paths = vfs::BuildTTFPathList(font.Family());
            for(const vfs::path &path : paths) {
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
            try {
                const FontData &fontData = GetFontContext(font);
                return IsCopyOf(fontData.Font(), font);
            } catch(const font_error &error) {
                return false;
            }
        }
    
        bool FontEngine::CouldRender(const core::Font &font, const std::string &text) const
        {
            try {
                const FontData &fontData = GetFontContext(font);
                for(auto character : text) {
                    if(!fontData.HasGlyph(character)) {
                        return false;
                    }
                }
                return true;
            } catch(const font_error &error) {
                return false;
            }
        }

        void FontEngine::AddFontData(FontData fontdata)
        {
            mFontTable.push_back(std::move(fontdata));
        }
    
        bool FontEngine::DrawText(RenderEngine &engine, const core::Point &target, const core::Font &font, const std::string &text, const core::Color &fg, const core::Color &bg) const
        {
            if(!text.empty()) {
                const FontData &fontData = GetFontContext(font);

                /**
                   \todo consider using FontData::RenderShaded and FontData::RenderSolid
                **/
                core::Image textImage = fontData.RenderBlended(text.c_str(), fg);
                if(!textImage.Null()) {
                    if(bg.a != 0) {
                        const core::Rect bgRect(target.X(), target.Y(), textImage.Width(), textImage.Height());
                        engine.DrawRects(&bgRect, 1, bg, castle::render::DrawMode::Filled);
                    }
                    engine.SetOpacityMod(fg.a);
                    engine.DrawImage(textImage, core::Rect(textImage.Width(), textImage.Height()), target);
                } else {
                    throw ttf_error();
                }

                return true;
            }

            return false;
        }

        const core::Size FontEngine::TextSize(const core::Font &font, const std::string &text) const
        {
            return GetFontContext(font).TextSize(text);
        }

        int FontEngine::LineSkip(const core::Font &font) const
        {
            return GetFontContext(font).LineSkip();
        }
    } // namespace render
}

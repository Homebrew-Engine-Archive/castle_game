#include "fontmanager.h"

#include <functional>
#include <algorithm>

#include <boost/filesystem/fstream.hpp>
#include <game/rw.h>
#include <game/sdl_utils.h>

struct CloseFontDeleter
{
    void operator()(TTF_Font *font) {
        if(font != nullptr) {
            TTF_CloseFont(font);
        }
    }
};

struct FontData
{
    fs::path fontPath;
    std::string name;
    int fsize;
    std::unique_ptr<TTF_Font, CloseFontDeleter> font;

    FontData() = default;
    FontData(FontData&&) = default;
};

namespace Render
{
    FontManager::FontManager()
    {
        TTF_Init();
    }
    
    FontManager::~FontManager()
    {
        mFontTable.clear();
        TTF_Quit();
    }

    FontManager& FontManager::operator=(FontManager&&) = default;
    FontManager::FontManager(FontManager&&) = default;

    FontData* FontManager::GetBestMatch(const std::string &name, int fsize, FontData *lhs, FontData *rhs)
    {
        if(lhs == nullptr)
            return rhs;
        
        if(rhs == nullptr)
            return lhs;
        
        if((lhs->name == name) && (rhs->name != name))
            return lhs;
        
        if((lhs->name != name) && (rhs->name == name))
            return rhs;

        if(abs(lhs->fsize - fsize) < abs(rhs->fsize - fsize)) {
            return lhs;
        } else {
            return rhs;
        }

        throw std::runtime_error("Where is my old riffle?!!");
    }
    
    FontData* FontManager::LookupFontName(const std::string &name, int fsize)
    {
        FontData *result = nullptr;
        for(FontData &fd : mFontTable) {
            result = GetBestMatch(name, fsize, result, &fd);
        }
        return result;
    }
    
    void FontManager::LoadFont(const std::string &name, int fsize)
    {
        FontData *nearest = LookupFontName(name, fsize);
        if((nearest != nullptr) && (nearest->name == name) && (nearest->fsize == fsize)) {
            throw std::runtime_error("Font already loaded");
        }
        
        FontData fontData;
        fontData.fontPath = fs::FontFilePath(name);
        fontData.name = name;
        fontData.fsize = fsize;

        // TODO windows sucks so much with utf8
        // see http://stackoverflow.com/questions/11352641/boostfilesystempath-and-fopen

        // NOTE There is a bug with SDL_OpenFontRW
        // see http://forums.libsdl.org/viewtopic.php?t=8050&sid=ba3720be045e8acadf2645d7369156f8

        // The desired solutions is as follows
        //
        // boost::filesystem::ifstream fin(fontData.fontPath);
        // RWPtr rw(SDL_RWFromInputStream(fin));
        // TTF_Font *font = TTF_OpenFontRW(rw.get(), SDL_FALSE, fsize);
        //
        // ... but it get a crash
        
        TTF_Font *font = TTF_OpenFont(fontData.fontPath.string().c_str(), fsize);

        fontData.font.reset(font);
        if(!fontData.font) {
            throw std::runtime_error(TTF_GetError());
        }
        
        mFontTable.emplace_back(std::move(fontData));
    }
    
    TTF_Font* FontManager::FindFont(const std::string &name, int fsize)
    {
        const FontData *fd = LookupFontName(name, fsize);
        if(fd != nullptr) {
            return fd->font.get();
        }
        throw std::runtime_error("No font with such name");
    }
}

#include "fontmanager.h"

#include <stdexcept>
#include <string>
#include <iostream>

#include <boost/filesystem/fstream.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <game/ttf_error.h>
#include <game/sdl_utils.h>

struct FontData
{
    fs::path fontPath;
    std::string name;
    int fsize;
    FontPtr font;

    FontData() = default;
    FontData(FontData&&) = default;
};

namespace Render
{
    FontManager::~FontManager() = default;
    
    FontManager::FontManager()
        : mTTF_Init()
        , mFontTable()
    {
    }
    
    FontData* GetBestMatch(const std::string &name, int fsize, FontData *lhs, FontData *rhs)
    {
        if(lhs == nullptr) {
            return rhs;
        }
        
        if(rhs == nullptr) {
            return lhs;
        }
        
        if((lhs->name == name) && (rhs->name != name)) {
            return lhs;
        }
        
        if((lhs->name != name) && (rhs->name == name)) {
            return rhs;
        }

        if(abs(lhs->fsize - fsize) < abs(rhs->fsize - fsize)) {
            return lhs;
        } else {
            return rhs;
        }
    }
    
    FontData* FontManager::LookupFontName(const std::string &name, int fsize)
    {
        FontData *result = nullptr;
        for(FontData &fd : mFontTable) {
            result = GetBestMatch(name, fsize, result, &fd);
        }
        return result;
    }
    
    void FontManager::LoadFontFile(const std::string &name, int fsize)
    {
        FontData *nearest = LookupFontName(name, fsize);
        if((nearest != nullptr) && (nearest->name == name) && (nearest->fsize == fsize)) {
            std::clog << "Loading font has already been loaded: " << name << ' ' << fsize << std::endl;
            return;
        }
        
        FontData fontData;
        fontData.fontPath = fs::FontFilePath(name);
        fontData.name = name;
        fontData.fsize = fsize;

        // TODO windows sucks with utf8 so much
        // see http://stackoverflow.com/questions/11352641/boostfilesystempath-and-fopen
        // so we must not use c_str().
        
        // The desired solution is as follows
        //
        // boost::filesystem::ifstream fin(fontData.fontPath);
        // RWPtr rw(SDL_RWFromInputStream(fin));
        // TTF_Font *font = TTF_OpenFontRW(rw.get(), SDL_FALSE, fsize);

        // ... but it gets a crash due to a bug in SDL_OpenFontRW
        // see http://forums.libsdl.org/viewtopic.php?t=8050&sid=ba3720be045e8acadf2645d7369156f8



        TTF_Font *font = TTF_OpenFont(fontData.fontPath.string().c_str(), fsize);

        if(font == NULL) {
            throw ttf_error();
        }
        fontData.font.reset(font);
        
        mFontTable.push_back(std::move(fontData));
    }
    
    TTF_Font* FontManager::Font(const std::string &name, int fsize)
    {
        const FontData *fd = LookupFontName(name, fsize);
        if(fd != nullptr) {
            return fd->font.get();
        }
        throw std::runtime_error("no font found");
    }

    FontManager& FontManager::Instance()
    {
        static FontManager manager;
        return manager;
    }

    TTF_Font* FindFont(const std::string &fname, int fsize)
    {
        return FontManager::Instance().Font(fname, fsize);
    }

    void LoadFont(const std::string &fname, int fsize)
    {
        FontManager::Instance().LoadFontFile(fname, fsize);
    }
}

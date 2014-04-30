#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <game/filesystem.h>

#include <map>

#include <SDL.h>
#include <SDL_ttf.h>

class FontData;

namespace Render
{
    const std::string FontStronghold = "DejaVuSans";
    
    class FontManager
    {
        std::vector<FontData> mFontTable;

        FontData* GetBestMatch(const std::string &name, int fsize, FontData *lhs, FontData *rhs);
        FontData* LookupFontName(const std::string &name, int fsize);
        
    public:
        FontManager();
        FontManager(FontManager const&) = delete;
        FontManager& operator=(FontManager const&) = delete;
        FontManager& operator=(FontManager&&);
        FontManager(FontManager&&);
        virtual ~FontManager();
        
        virtual void LoadFont(const std::string &name, int fsize);
        virtual TTF_Font* FindFont(const std::string &name, int fsize);
    };
}

#endif  // FONTMANAGER_H_

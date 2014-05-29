#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <game/filesystem.h>
#include <game/ttf_utils.h>

#include <SDL_ttf.h>

#include <vector>
#include <string>

class FontData;

namespace Render
{
    const std::string FontStronghold = "DejaVuSans";
    
    class FontManager
    {
        TTFInitializer mTTF_Init;
        std::vector<FontData> mFontTable;

        FontData* LookupFontName(const std::string &name, int fsize);
        
    public:
        FontManager();
        FontManager(FontManager const&) = delete;
        FontManager& operator=(FontManager const&) = delete;
        virtual ~FontManager();

        static FontManager& Instance();
        
        void LoadFontFile(const std::string &name, int fsize);
        TTF_Font* Font(const std::string &name, int fsize);
    };

    TTF_Font* FindFont(const std::string &name, int fsize);
    void LoadFont(const std::string &name, int fsize);
}

#endif  // FONTMANAGER_H_

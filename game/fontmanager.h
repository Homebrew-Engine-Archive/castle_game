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
        FontData *mDefaultFontData;

        FontData* GetBestMatch(const std::string &name, int fsize, FontData *lhs, FontData *rhs);
        FontData* LookupFontName(const std::string &name, int fsize);

        void CheckDefaultFontIsSet() const;
        
    public:
        FontManager();
        FontManager& operator=(FontManager&&);
        FontManager(FontManager&&);
        FontManager(FontManager const&) = delete;
        FontManager& operator=(FontManager const&) = delete;
        virtual ~FontManager();
        
        void LoadFontFile(const std::string &name, int fsize);
        TTF_Font* Font(const std::string &name, int fsize);

        void SetDefaultFont(const std::string &name, int fsize);
        
        TTF_Font* DefaultFont();
        std::string DefaultFontName() const;
        int DefaultFontSize() const;
    };
}

#endif  // FONTMANAGER_H_

#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <vector>
#include <string>

#include <game/filesystem.h>

class TTFInitializer;
class Color;
class Rect;
class FontData;

namespace UI
{
    class Font;
}

namespace Render
{
    class Renderer;
}

namespace Render
{
    const std::string RegularFont = "DejaVuSans";
    
    class FontManager
    {
    public:
        FontManager();
        FontManager(FontManager const&) = delete;
        FontManager& operator=(FontManager const&) = delete;
        virtual ~FontManager();

        void DrawText(Renderer &renderer, const UI::Font &font, const std::string &text, const Color &fg, const Color &bg) const;
        void DrawTextBoxed(Renderer &renderer, const UI::Font &font, const std::string &text, const Color &fg, const Color &bg) const;        
        bool CouldRender(const UI::Font &font, const std::string &text) const;
        void LoadFont(const UI::Font &font);
        const Rect TextSize(const UI::Font &font, const std::string &text) const;
        int LineSkip(const UI::Font &font) const;

    private:
        std::unique_ptr<TTFInitializer> mTTF_Init;
        std::vector<FontData> mFontTable;
        
    protected:
        void AddFontData(FontData fontdata);
        std::vector<fs::path> FontSearchPathsList(const UI::Font &font) const;
        bool FontIsLoaded(const UI::Font &font) const;
        const FontData& LookupFont(const UI::Font &font) const;
        FontData LoadFontData(const fs::path &path, const UI::Font &font) const;
    };
}

#endif  // FONTMANAGER_H_

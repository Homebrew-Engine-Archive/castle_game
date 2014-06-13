#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <vector>
#include <string>

#include <game/filesystem.h>

class TTFInitializer;
class Color;
class Rect;
class FontData;

namespace core
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

        void DrawText(Renderer &renderer, const core::Font &font, const std::string &text, const Color &fg, const Color &bg) const;
        bool CouldRender(const core::Font &font, const std::string &text) const;
        bool LoadFont(const core::Font &font);
        const Rect TextSize(const core::Font &font, const std::string &text) const;
        int LineSkip(const core::Font &font) const;

    protected:
        std::unique_ptr<TTFInitializer> mTTF_Init;
        std::vector<FontData> mFontTable;
        
    private:
        void AddFontData(FontData fontdata);
        std::vector<fs::path> FontSearchPathsList(const core::Font &font) const;
        FontData const* LookupFont(const core::Font &font) const;
        bool HasFontLoaded(const core::Font &font) const;
        bool HasExactMatch(const core::Font &font) const;
        FontData LoadFontData(const fs::path &path, const core::Font &font) const;
    };
}

#endif  // FONTMANAGER_H_

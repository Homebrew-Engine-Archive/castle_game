#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <vector>
#include <string>

#include <game/filesystem.h>

class TTFInitializer;
class FontData;

namespace core
{
    class Color;
    class Rect;
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
        explicit FontManager();
        FontManager(FontManager const&);
        FontManager& operator=(FontManager const&);
        FontManager(FontManager&&);
        FontManager& operator=(FontManager&&);
        virtual ~FontManager();

        void DrawText(Renderer &renderer, const core::Font &font, const std::string &text, const core::Color &fg, const core::Color &bg) const;
        bool CouldRender(const core::Font &font, const std::string &text) const;
        bool LoadFont(const core::Font &font);
        const core::Rect TextSize(const core::Font &font, const std::string &text) const;
        int LineSkip(const core::Font &font) const;

    private:
        std::unique_ptr<TTFInitializer> mTTF_Init;
        std::vector<FontData> mFontTable;
        
    protected:
        void AddFontData(FontData fontdata);
        std::vector<fs::path> FontSearchPathsList(const core::Font &font) const;
        FontData const* LookupFont(const core::Font &font) const;
        bool HasFontLoaded(const core::Font &font) const;
        bool HasExactMatch(const core::Font &font) const;
        FontData LoadFontData(const fs::path &path, const core::Font &font) const;
    };
}

#endif  // FONTMANAGER_H_

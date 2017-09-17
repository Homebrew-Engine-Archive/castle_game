#ifndef FONTENGINE_H_
#define FONTENGINE_H_

#include <vector>
#include <string>

#include <game/vfs.h>
#include <game/fontdata.h>

class TTFInitializer;
//class FontData;

namespace core
{
    class Size;
    class Color;
    class Font;
    class Point;
}

namespace castle
{
    namespace render
    {
        class RenderEngine;
    }

    namespace render
    {
        const std::string RegularFont = "DejaVuSans";

        struct font_error : public virtual std::exception
        {
            const char* what() const throw();
        };
        
        class FontEngine
        {
        public:
            FontEngine(FontEngine const&) = delete;
            FontEngine& operator=(FontEngine const&) = delete;
            FontEngine(FontEngine&&) = delete;
            FontEngine& operator=(FontEngine&&) = delete;

            explicit FontEngine();
            virtual ~FontEngine() = default;

            bool DrawText(RenderEngine &engine, const core::Point &target, const core::Font &font, const std::string &text, const core::Color &fg, const core::Color &bg) const;
            bool CouldRender(const core::Font &font, const std::string &text) const;
            bool LoadFont(const core::Font &font);
            const core::Size TextSize(const core::Font &font, const std::string &text) const;
            int LineSkip(const core::Font &font) const;

        private:
            std::unique_ptr<TTFInitializer> mTTF_Init;
            std::vector<FontData> mFontTable;
        
        protected:
            void AddFontData(FontData fontdata);
            FontData const* LookupFont(const core::Font &font) const;
            const FontData& GetFontContext(const core::Font &font) const;
            bool HasFontLoaded(const core::Font &font) const;
            bool HasExactMatch(const core::Font &font) const;
            FontData LoadFontData(const vfs::path &path, const core::Font &font) const;
        };
    }
}

#endif  // FONTENGINE_H_

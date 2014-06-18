#ifndef RENDERMODE_H_
#define RENDERMODE_H_

#include <iosfwd>
#include <string>
#include <vector>
#include <memory>

#include <boost/filesystem/path.hpp>

#include <gmtool/mode.h>

#include <game/color.h>

namespace Castle
{
    class Image;
    class Palette;
}

namespace GMTool
{
    class RenderFormat;
}

namespace GMTool
{
    class RenderMode : public Mode
    {
        boost::filesystem::path mInputFile;
        boost::filesystem::path mOutputFile;
        std::string mFormat;
        int mEntryIndex = 0;
        int mPaletteIndex = 0;
        core::Color mTransparentColor;
        std::vector<RenderFormat> mFormats;
        bool mApproxSize = false;

        const core::Color DefaultTransparent() const;
        
        void SetupFormat(Castle::Image &surface, uint32_t format);
        void SetupPalette(Castle::Image &surface, const Castle::Palette &palette);
        void SetupTransparentColor(Castle::Image &surface, const core::Color &color);
        
    public:
        RenderMode();
        virtual ~RenderMode() throw();
        
        void PrintUsage(std::ostream &out);
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif  // RENDERMODE_H_

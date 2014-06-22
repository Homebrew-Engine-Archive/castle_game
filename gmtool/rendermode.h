#ifndef RENDERMODE_H_
#define RENDERMODE_H_

#include <iosfwd>
#include <string>
#include <vector>
#include <memory>

#include <boost/filesystem/path.hpp>

#include <gmtool/mode.h>

#include <game/color.h>

namespace castle
{
    class Image;
    class Palette;
}

namespace gmtool
{
    class RenderFormat;
}

namespace gmtool
{
    class RenderMode : public Mode
    {
        boost::filesystem::path mInputFile;
        boost::filesystem::path mOutputFile;
        std::string mFormat;
        size_t mEntryIndex = 0;
        size_t mPaletteIndex = 0;
        core::Color mTransparentColor;
        std::vector<RenderFormat> mFormats;
        bool mEvalSizeOnly = false;

        const core::Color DefaultTransparent() const;
        
        void SetupFormat(castle::Image &surface, uint32_t format);
        void SetupPalette(castle::Image &surface, const castle::Palette &palette);
        void SetupTransparentColor(castle::Image &surface, const core::Color &color);
        
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

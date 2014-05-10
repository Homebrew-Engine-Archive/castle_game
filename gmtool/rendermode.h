#ifndef RENDERMODE_H_
#define RENDERMODE_H_

#include <string>
#include <memory>

#include <boost/filesystem/path.hpp>

#include <gmtool/mode.h>
#include <gmtool/renderer.h>
#include <gmtool/hexcolor.h>

#include <game/color.h>
#include <game/gm1palette.h>

class Surface;

namespace GMTool
{
    class RenderMode : public Mode
    {
        boost::filesystem::path mInputFile;
        boost::filesystem::path mOutputFile;
        std::string mFormat;
        int mEntryIndex = 0;
        int mPaletteIndex = 0;
        Color mTransparentColor;
        std::vector<RenderFormat> mFormats;
        bool mApproxSize = false;

        Color DefaultTransparent() const;
        
        void SetupFormat(Surface &surface, const PixelFormatPtr &format);
        void SetupPalette(Surface &surface, const GM1::Palette &palette);
        void SetupTransparentColor(Surface &surface, const Color &color);
        
    public:
        RenderMode();
        void PrintUsage(std::ostream &out);
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif  // RENDERMODE_H_

#ifndef RENDERMODE_H_
#define RENDERMODE_H_

#include <string>

#include <boost/filesystem/path.hpp>

#include "colorvalidator.h"
#include "mode.h"

namespace GMTool
{
    class RenderMode : public Mode
    {
        boost::filesystem::path mInputFile;
        std::string mFormat;
        bool mTileOnly;
        bool mBoxOnly;
        int mEntryIndex;
        int mPaletteIndex;
        ColorValue mTransparentColor;
    public:
        void PrintUsage(std::ostream &out);
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif  // RENDERMODE_H_

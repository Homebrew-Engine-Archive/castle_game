#ifndef PALETTEMODE_H_
#define PALETTEMODE_H_

#include "mode.h"
#include <boost/filesystem/path.hpp>

namespace GMTool
{
    class PaletteMode : public Mode
    {
        boost::filesystem::path mInputFile;
        int mPaletteIndex;
        bool mBinary;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif // PALETTEMODE_H_

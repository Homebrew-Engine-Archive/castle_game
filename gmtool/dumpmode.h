#ifndef DUMPMODE_H_
#define DUMPMODE_H_

#include <boost/filesystem/path.hpp>

#include <gmtool/mode.h>
#include <gmtool/tilepart.h>

namespace gmtool
{
    class DumpMode : public Mode
    {
        boost::filesystem::path mInputFile;
        size_t mEntryIndex;
        TilePart mTilePart;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif

#ifndef DUMPMODE_H_
#define DUMPMODE_H_

#include <gmtool/mode.h>
#include <boost/filesystem/path.hpp>

namespace GMTool
{
    class DumpMode : public Mode
    {
        boost::filesystem::path mInputFile;
        int mEntryIndex;
        bool mTileOnly;
        bool mBoxOnly;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif

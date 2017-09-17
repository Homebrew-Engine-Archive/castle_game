#ifndef ENTRYMODE_H_
#define ENTRYMODE_H_

#include <gmtool/mode.h>

#include <boost/filesystem/path.hpp>

namespace gmtool
{
    class EntryMode : public Mode
    {
        boost::filesystem::path mInputFile;
        size_t mEntryIndex;
        bool mBinary;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif // ENTRYMODE_H_
 

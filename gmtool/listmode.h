#ifndef LISTMODE_H_
#define LISTMODE_H_

#include "mode.h"

#include <iosfwd>

#include <boost/filesystem/path.hpp>

namespace GM1
{
    class GM1Reader;
}

namespace GMTool
{
    void ShowEntryList(std::ostream &out, const GM1::GM1Reader &reader);

    class ListMode : public Mode
    {
        boost::filesystem::path mInputFile;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &cfg);
    };
}

#endif // LISTMODE_H_

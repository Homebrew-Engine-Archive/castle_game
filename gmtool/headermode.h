#ifndef HEADERMODE_H_
#define HEADERMODE_H_

#include <iostream>
#include <gmtool/mode.h>
#include <boost/filesystem/path.hpp>
 
namespace gmtool
{
    class HeaderMode : public Mode
    {
        boost::filesystem::path mInputFile;
        bool mBinary = false;
        bool mCountRequested = false;
        bool mReaderTypeRequested = false;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif // HEADERMODE_H_

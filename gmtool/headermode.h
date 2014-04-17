#ifndef HEADERMODE_H_
#define HEADERMODE_H_

#include "mode.h"

#include <iostream>

namespace GMTool
{
    class HeaderMode : public Mode
    {
        std::string mInputFile;
        bool mBinary;
    public:
        void GetOptions(boost::program_options::options_description&);
        void GetPositionalOptions(boost::program_options::positional_options_description&);
        int Exec(const ModeConfig &config);
    };
}

#endif // HEADERMODE_H_

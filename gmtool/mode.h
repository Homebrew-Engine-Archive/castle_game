#ifndef MODE_H_
#define MODE_H_

#include <iosfwd>
#include <memory>

/**
   boost::filesystem::operator>> overload for boost::filesystem::path
**/
#include <gmtool/path_validator.h>

namespace boost
{
    namespace program_options
    {
        class options_description;
        class positional_options_description;
    }
}

namespace gmtool
{
    struct ModeConfig
    {
        bool helpRequested;
        bool versionRequested;
        bool verboseRequested;
        std::ostream &verbose;
        std::ostream &stdout;
    };

    class Mode
    {
    public:
        typedef std::shared_ptr<Mode> Ptr;

        virtual void PrintUsage(std::ostream&) {}

        virtual ~Mode() = default;
        virtual void GetOptions(boost::program_options::options_description&) {};
        virtual void GetPositionalOptions(boost::program_options::positional_options_description&) {};
        virtual int Exec(const ModeConfig &config) = 0;
    };
}

#endif

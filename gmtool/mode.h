#ifndef MODE_H_
#define MODE_H_

#include <iosfwd>
#include <memory>

namespace boost
{
    namespace program_options
    {
        class options_description;
        class positional_options_description;
    }
}

/**
   boost::filesystem::operator>> overload for boost::filesystem::path
**/
#include <gmtool/path_validator.h>

namespace GMTool
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
        virtual void GetOptions(boost::program_options::options_description&) = 0;
        virtual void GetPositionalOptions(boost::program_options::positional_options_description&) = 0;
        virtual int Exec(const ModeConfig &config) = 0;
    };
}

#endif

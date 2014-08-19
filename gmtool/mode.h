#ifndef MODE_H_
#define MODE_H_

#include <iosfwd>
#include <memory>

// Overload of operator>> for boost::filesystem::path.
// Placed here to hide default implementation.
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
        std::ostream &verbose;
        std::ostream &stdout;
    };

    /// Basic command mode.
    class Mode
    {
    public:
        virtual ~Mode() = default;
        typedef std::shared_ptr<Mode> Ptr;

        virtual void PrintUsage(std::ostream &out) {}
        
        virtual void GetOptions(boost::program_options::options_description&) {};
        ///< Called once per mode in order to construct command line parser.
        
        virtual void GetPositionalOptions(boost::program_options::positional_options_description&) {};
        ///< Exactly the same as GetOptions but for the positional options.
        
        virtual int Exec(const ModeConfig &config) = 0;
    };
}

#endif

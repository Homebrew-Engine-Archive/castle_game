#ifndef MODEHANDLER_H_
#define MODEHANDLER_H_

#include <string>
#include <boost/program_options.hpp>

namespace GMTool
{

    class ModeHandler
    {
    public:
        virtual void RegisterOptions(boost::program_options::options_description &options) = 0;

        /**
         * \brief String as one can type it into console
         */
        virtual std::string ModeName() const = 0;
        
        virtual int HandleMode(const boost::program_options::variables_map &vars) = 0;
    };
    
}

#endif

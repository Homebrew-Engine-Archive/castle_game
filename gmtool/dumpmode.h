#ifndef DUMPMODE_H_
#define DUMPMODE_H_

#include "modehandler.h"
#include <string>
#include <boost/program_options.hpp>

namespace GMTool
{

    class DumpMode : public ModeHandler
    {
    public:
        virtual std::string ModeName() const;

        virtual int HandleMode(const boost::program_options::variables_map &vars);
        
        virtual void RegisterOptions(boost::program_options::options_description &descr);
    };
    
}

#endif

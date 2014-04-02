#ifndef INFOMODE_H_
#define INFOMODE_H_

#include "modehandler.h"
#include <boost/program_options.hpp>

namespace GMTool
{

    class InfoMode : public ModeHandler
    {
    public:
        virtual void RegisterOptions(boost::program_options::options_description &options);
        
        virtual std::string ModeName() const;
        virtual int HandleMode(const boost::program_options::variables_map &vars);
    };
    
}

#endif

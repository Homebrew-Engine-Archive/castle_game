#ifndef TGXRENDERMODE_H_
#define TGXRENDERMODE_H_

#include <boost/program_options.hpp>
#include "modehandler.h"

namespace GMTool
{

    class TGXRenderMode : public ModeHandler
    {
    public:
        virtual void RegisterOptions(boost::program_options::options_description &desc);
        virtual std::string ModeName() const;
        virtual int HandleMode(const boost::program_options::variables_map &vars);
    };
    
}

#endif

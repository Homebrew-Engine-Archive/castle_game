#ifndef RENDERMODE_H_
#define RENDERMODE_H_

#include "modehandler.h"
#include <boost/program_options.hpp>
#include <string>

namespace GMTool
{

    class RenderMode : public ModeHandler
    {
    public:
        void RegisterOptions(boost::program_options::options_description &desc);
        
        std::string ModeName() const;
        int HandleMode(const boost::program_options::variables_map &vars);
    };
    
}

#endif

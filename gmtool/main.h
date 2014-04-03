#ifndef MAIN_H_
#define MAIN_H_

#include <boost/program_options.hpp>
#include "modehandler.h"

namespace GMTool
{
    
    class ToolMain
    {
        boost::program_options::variables_map mVars;
        std::vector<std::unique_ptr<ModeHandler>> mHandlers;

        std::string ModeLine() const;
        
        void RegisterModes();

    public:
        ToolMain(int argc, const char *argv[]);
        int Exec();
    };

}

#endif

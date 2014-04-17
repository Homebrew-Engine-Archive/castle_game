#ifndef MAIN_H_
#define MAIN_H_

#include <boost/program_options.hpp>

#include "mode.h"

namespace GMTool
{
    class Command;
}

namespace GMTool
{
    class ToolMain
    {
        std::vector<Command> GetCommands();

        int RunCommand(const std::vector<std::string> &args, Command const&, ModeConfig const&);

    public:
        int Exec(int argc, const char *argv[]);
    };

}

#endif

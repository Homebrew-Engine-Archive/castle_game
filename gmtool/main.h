#ifndef MAIN_H_
#define MAIN_H_

#include <gmtool/mode.h>

namespace gmtool
{
    class Command;
}

namespace gmtool
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

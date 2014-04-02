#ifndef MAIN_H_
#define MAIN_H_

#include <boost/program_options.hpp>

namespace GMTool
{

    class ToolMain
    {
        boost::program_options::variables_map vars;
        int Dump();
        int Info();
        int Render();
    public:
        ToolMain(int argc, const char *argv[]);
        int Exec();
    };

}

#endif

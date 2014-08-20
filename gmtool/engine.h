#ifndef ENGINE_H_
#define ENGINE_H_
    
#include <vector>
#include <string>
#include <gmtool/mode.h>

namespace gmtool
{
    class Engine
    {
        bool mHelpRequested = false;
        bool mVerboseRequested = false;
        bool mVersionRequested = false;
        std::string mModeName;

        int RunMode(Mode &mode, const std::vector<std::string> &tokens);
    public:
        int Exec(int argc, const char *argv[]);
    };
}

#endif // ENGINE_H_

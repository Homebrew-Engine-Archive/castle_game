#ifndef ENGINE_H_
#define ENGINE_H_
    
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
    public:
        int Exec(int argc, const char *argv[]);
    };
}

#endif // ENGINE_H_

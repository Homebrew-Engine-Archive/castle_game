#ifndef ENGINE_H_
#define ENGINE_H_
    
#include <gmtool/mode.h>

namespace gmtool
{
    class Engine
    {
    public:
        int Exec(int argc, const char *argv[]);
    };
}

#endif // ENGINE_H_

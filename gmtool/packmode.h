#ifndef PACKMODE_H_
#define PACKMODE_H_

#include "mode.h"

namespace gmtool
{
    class PackMode : public Mode
    {
    public:
        virtual int Exec(const ModeConfig &config);
    };
}

#endif // PACKMODE_H_

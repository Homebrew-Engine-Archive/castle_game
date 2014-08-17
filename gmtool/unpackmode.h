#ifndef UNPACKMODE_H_
#define UNPACKMODE_H_

#include "mode.h"

namespace gmtool
{
    class UnpackMode : public Mode
    {
    public:
        virtual int Exec(const ModeConfig &config);
    };
}

#endif // UNPACKMODE_H_

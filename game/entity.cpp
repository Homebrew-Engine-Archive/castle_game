#include "entity.h"

#include <string>

#include <game/direction.h>

namespace
{
    struct AnimationGroup
    {
        std::string name;
        size_t size;
        core::DirectionSet dirs;
    };    
}

namespace Entities
{
    std::vector<AnimationGroup> BodyArcherInfo = {
        {"walk", 16, core::EightDirs},
        {"run", 16, core::EightDirs},
        {"shot", 24, core::EightDirs},
        {"shotdown", 12, core::EightDirs},
        {"shotup", 12, core::EightDirs},
        {"tilt", 12, core::FourDirs},
        {"aware", 16, core::SingleDir},
        {"idle", 16, core::SingleDir},
        {"deatharrow", 24, core::SingleDir},
        {"deathmelee1", 24, core::SingleDir},
        {"deathmelee2", 24, core::SingleDir},
        {"attack", 12, core::EightDirs},
        {"climb", 12, core::EightDirs},
        {"fall", 8, core::EightDirs},
        {"dig", 16, core::EightDirs}
    };
}

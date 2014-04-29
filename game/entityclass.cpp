#include "entityclass.h"

#include <string>

#include <game/animation.h>
#include <game/direction.h>

namespace
{
    struct AnimationGroup
    {
        std::string name;
        size_t size;
        DirectionSet dirs;
    };    
}

namespace Entities
{
    std::vector<AnimationGroup> ArcherAnimationGroups = {
        {"walk", 16, EightDirs},
        {"run", 16, EightDirs},
        {"shot", 24, EightDirs},
        {"shotdown", 12, EightDirs},
        {"shotup", 12, EightDirs},
        {"tilt", 12, FourDirs},
        {"aware", 16, SingleDir},
        {"idle", 16, SingleDir},
        {"deatharrow", 24, SingleDir},
        {"deathmelee1", 24, SingleDir},
        {"deathmelee2", 24, SingleDir},
        {"attack", 12, EightDirs},
        {"climb", 12, EightDirs},
        {"fall", 8, EightDirs},
        {"dig", 16, EightDirs}
    };
}

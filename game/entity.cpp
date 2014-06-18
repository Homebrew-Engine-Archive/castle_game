#include "entity.h"

#include <string>

#include <game/direction.h>

namespace Castle
{
    namespace World
    {
        struct AnimationGroup
        {
            std::string name;
            size_t size;
            core::DirectionSet dirs;
        };    
        
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

        std::vector<AnimationGroup> BodyPeasantInfo = {
            {"walk", 16, core::EightDirs},
            {"idle", 4, core::FourDirs},
            {"sit_down", 8, core::FourDirs},
            {"emotion", 12, core::FourDirs},
            {"idle2", 20, core::FourDirs},
            {"idle3", 10, core::FourDirs},
            {"reverance", 12, core::FourDirs},
            {"deatharrow", 24, core::SingleDir},
            {"death", 24, core::SingleDir},
            {"deathkick", 23, core::SingleDir}
        };
    }
}

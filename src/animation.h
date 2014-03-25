#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include "direction.h"
#include "gm1.h"
#include "surface.h"
#include "filesystem.h"
#include "resourcemanager.h"

enum class AnimationClass : uint32_t {
    Walk,
    Run,
    Idle1,
    Idle2,
    Idle3,
    Dig,
    Climb,
    RemoveLadder,
    DeathArrow,
    DeathMelee1,
    DeathMelee2,
    Fall,
    AttackMelee,
    Victory                                              // swordman, mace
};

namespace Render
{
    class ResourceManager;
}

namespace {

    struct SpriteGroup
    {
        std::string collectionName;
        std::string name;
        int nize;
        DirectionSet dirs;
    };

    class Entity {};

    class Archer : public Entity
    {
    public:
        std::string GetArchiveName() const;
        bool HasSpriteGroup(const std::string &name) const;
        
    };
    
    void BuildArcher(Render::ResourceManager *manager)
    {
        static const SpriteGroup groups[] = {
            {"body_archer", "walk", 16, FullDirectionSet},
            {"body_archer", "run", 16, FullDirectionSet},
            {"body_archer", "shot", 24, FullDirectionSet},
            {"body_archer", "shotdown", 12, FullDirectionSet},
            {"body_archer", "shotup", 12, FullDirectionSet},
            {"body_archer", "tilt", 12, MainDirectionSet},
            {"body_archer", "aware", 16, SingleDirectionSet},
            {"body_archer", "idle", 16, SingleDirectionSet},
            {"body_archer", "deatharrow", 24, SingleDirectionSet},
            {"body_archer", "deathmelee1", 24, SingleDirectionSet},
            {"body_archer", "deathmelee2", 24, SingleDirectionSet},
            {"body_archer", "attack", 12, FullDirectionSet},
            {"body_archer", "climb", 12, FullDirectionSet},
            {"body_archer", "fall", 8, FullDirectionSet},
            {"body_archer", "dig", 16, FullDirectionSet}
        };

        size_t skip = 0;
        for(const SpriteGroup &group : groups) {
            for(size_t index = 0; index < group.size; ++index) {
                for(const Direction &dir : group.dirs) {
                    manager->AddSprite(group.collectionName, skip, group.name, dir, index);
                    ++skip;
                }
            }
        }
    }
    
}

#endif

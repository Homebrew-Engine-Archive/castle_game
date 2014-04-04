#include "entityclass.h"

#include <string>

#include <game/animation.h>
#include <game/graphicsmanager.h>
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
    
    class UnitEntity : public EntityClass
    {
    protected:
        virtual std::vector<AnimationGroup> AnimationGroups() const = 0;
        virtual std::string ArchiveName() const = 0;
    public:
        virtual void LoadGraphics(Render::GraphicsManager &manager);
    };

    void UnitEntity::LoadGraphics(Render::GraphicsManager &manager)
    {
        size_t skip = 0;
        for(AnimationGroup const& group : AnimationGroups()) {
            for(size_t index = 0; index < group.size; ++index) {
                for(Direction const& dir : group.dirs) {
                    manager.AddGraphics(ArchiveName(), skip);
                    ++skip;
                }
            }
        }
    }

    class ArcherEntity : public UnitEntity
    {
    protected:
        virtual std::vector<AnimationGroup> AnimationGroups() const;
        virtual std::string ArchiveName() const;
    };

    std::vector<AnimationGroup> ArcherEntity::AnimationGroups() const
    {
        std::vector<AnimationGroup> groups = {
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
        return groups;
    }

    std::string ArcherEntity::ArchiveName() const
    {
        return "body_archer";
    }

    void LoadGraphics(Render::GraphicsManager &manager)
    {
        ArcherEntity archer;
        archer.LoadGraphics(manager);
    }

}

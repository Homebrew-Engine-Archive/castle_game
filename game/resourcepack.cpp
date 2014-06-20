#include "resourcepack.h"

#include <game/direction.h>

namespace castle
{
    namespace gfx
    {
        const DirSet EightDirs = DirSet {
            core::Direction::East,
            core::Direction::NorthEast,
            core::Direction::North,
            core::Direction::NorthWest,
            core::Direction::West,
            core::Direction::SouthWest,
            core::Direction::South,
            core::Direction::SouthEast
        };
    
        const DirSet FourDirs = DirSet {
            core::Direction::East,
            core::Direction::North,
            core::Direction::West,
            core::Direction::South
        };

        const DirSet SingleDir = DirSet {
            core::Direction::South
        };

        const DirSet Singleton(core::Direction dir)
        {
            return DirSet {dir};
        }
        
        std::vector<SpriteGroup> BodyArcherInfo = {
            {"walk", 16, EightDirs},
            {"run", 16, EightDirs},
            {"shot", 24, EightDirs},
            {"shotdown", 12, EightDirs},
            {"shotup", 12, EightDirs},
            {"push", 12, FourDirs},
            {"aware", 16, SingleDir},
            {"idle", 16, SingleDir},
            {"deatharrow", 24, SingleDir},
            {"deathmelee1", 24, SingleDir},
            {"deathmelee2", 24, SingleDir},
            {"attackmelee", 12, EightDirs},
            {"climb", 12, EightDirs},
            {"fall", 8, EightDirs},
            {"dig", 16, EightDirs}
        };

        std::vector<SpriteGroup> BodyPeasantInfo = {
            {"walk", 16, EightDirs},
            {"idle", 4, FourDirs},
            {"sit_down", 8, FourDirs},
            {"emotion", 12, FourDirs},
            {"idle2", 20, FourDirs},
            {"idle3", 10, FourDirs},
            {"reverance", 12, FourDirs},
            {"deatharrow", 24, SingleDir},
            {"death", 24, SingleDir},
            {"deathkick", 24, SingleDir}
        };

        std::vector<SpriteGroup> BodyLordInfo = {
            {"walk", 16, EightDirs},
            {"walk_with_sword", 16, EightDirs},
            {"talk", 24, EightDirs},
            {"attack1", 12, EightDirs},
            {"attack2", 12, EightDirs},
            {"attack3", 12, EightDirs},
            {"surrender", 28, SingleDir}
        };

        std::vector<SpriteGroup> BodySwordsmanInfo = {
            {"walk", 16, EightDirs},
            {"idle", 24, Singleton(core::Direction::NorthEast)},
            {"idle", 24, Singleton(core::Direction::SouthEast)},
            {"idle", 24, Singleton(core::Direction::SouthWest)},
            {"idle", 24, Singleton(core::Direction::NorthWest)},
            {"attack", 28, EightDirs},
            {"triumph", 16, FourDirs},
            {"deathmelee1", 24, SingleDir},
            {"deathmelee2", 24, SingleDir},
            {"deatharrow", 24, SingleDir}
        };
        
        castle::Image SpriteSequence::GetImage(const core::Direction &dir, float index)
        {
            return castle::Image();
        }

        ResourcePack::~ResourcePack() = default;
        ResourcePack::ResourcePack(const Collection &collection)
            : mCollection(collection)
        {
        }

        const SpriteSequence ResourcePack::ReadGroup(const std::string &groupName)
        {
            SpriteSequence tmp;
            
            return tmp;
        }

        void ResourcePack::AddGroup(const SpriteGroup &group)
        {
            mGroups.push_back(group);
        }
    }
}

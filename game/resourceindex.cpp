#include "resourceindex.h"

#include <algorithm>
#include <set>

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
        
        std::vector<ResourceGroup> BodyArcherInfo = {
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
        
        std::vector<ResourceGroup> BodyPeasantInfo = {
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

        std::vector<ResourceGroup> BodyLordInfo = {
            {"walk", 16, EightDirs},
            {"walk_with_sword", 16, EightDirs},
            {"talk", 24, EightDirs},
            {"attack1", 12, EightDirs},
            {"attack2", 12, EightDirs},
            {"attack3", 12, EightDirs},
            {"surrender", 28, SingleDir}
        };

        std::vector<ResourceGroup> BodySwordsmanInfo = {
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
        
        void IndexTable::AddIndex(const core::Direction &dir, size_t index)
        {
            mIndex[dir].push_back(index);
        }
        
        ResourceIndex::ResourceIndex(const std::vector<ResourceGroup> &groups)
        {
            size_t index = 0;
            for(const ResourceGroup &group : groups) {
                for(size_t i = 0; i < group.size; ++i) {
                    for(const core::Direction &dir : group.dirs) {
                        mTables[group.name].AddIndex(dir, index);
                        ++index;
                    }
                }
            }
        }

        const IndexTable& ResourceIndex::QueryIndex(const std::string &group) const
        {
            return mTables.at(group);
        }
    }
}

#include "spritelookuptable.h"

#include <stdexcept>
#include <algorithm>
#include <set>

#include <core/direction.h>

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
        
        std::vector<BodyGroupDescription> BodyArcherInfo = {
            {"walk", 16, EightDirs},
            {"run", 16, EightDirs},
            {"shoot", 24, EightDirs},
            {"shootdown", 12, EightDirs},
            {"shootup", 12, EightDirs},
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
        
        std::vector<BodyGroupDescription> BodyPeasantInfo = {
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

        std::vector<BodyGroupDescription> BodyLordInfo = {
            {"walk", 16, EightDirs},
            {"walk_with_sword", 16, EightDirs},
            {"talk", 24, EightDirs},
            {"attack1", 12, EightDirs},
            {"attack2", 12, EightDirs},
            {"attack3", 12, EightDirs},
            {"surrender", 28, SingleDir}
        };

        std::vector<BodyGroupDescription> BodySwordsmanInfo = {
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
        
        void DirectionTable::AddIndex(const core::Direction &dir, size_t index)
        {
            mIndex[dir].push_back(index);
        }
        
        const std::vector<size_t>& DirectionTable::QueryDirection(const core::Direction &dir) const
        {
            if(mIndex.empty()) {
                throw std::runtime_error("wrong query");
            }
            auto bestKv = mIndex.begin();
            for(auto kv = mIndex.begin(); kv != mIndex.end(); ++kv) {
                if(core::MinRotates(kv->first, dir) < core::MinRotates(bestKv->first, dir)) {
                    bestKv = kv;
                }
            }
            return bestKv->second;
        }
        
        BodyIndex::BodyIndex(const std::vector<BodyGroupDescription> &groups)
        {
            size_t index = 0;
            for(const BodyGroupDescription &group : groups) {
                for(size_t i = 0; i < group.frames; ++i) {
                    for(const core::Direction &dir : group.dirs) {
                        mTables[group.name].AddIndex(dir, index);
                        ++index;
                    }
                }
            }
        }

        const DirectionTable& BodyIndex::QueryGroup(const std::string &group) const
        {
            return mTables.at(group);
        }
    }
}

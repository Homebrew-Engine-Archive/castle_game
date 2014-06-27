#ifndef SPRITELOOKUPTABLE_H_
#define SPRITELOOKUPTABLE_H_

#include <map>
#include <string>
#include <vector>

#include <core/direction.h>

namespace castle
{
    namespace gfx
    {
        using DirSet = std::vector<core::Direction>;
        
        struct BodyGroupDescription
        {
            std::string name;
            size_t frames;
            DirSet dirs;
        };

        class DirectionTable
        {
            std::map<core::Direction, std::vector<size_t>> mIndex;
            
        public:
            void AddIndex(const core::Direction &dir, size_t index);
            const std::vector<size_t>& QueryDirection(const core::Direction &dir) const;
        };

        /**
           Entry index mapper from composite key (group, direction, frameNum)
           to sequential index.

           Class has been indented to use as helper for parsing body_*.gm1 collections.
         **/
        class BodyIndex
        {
            std::map<std::string, DirectionTable> mTables;
            
        public:
            explicit BodyIndex(const std::vector<BodyGroupDescription> &groups);
            const DirectionTable& QueryGroup(const std::string &group) const;
        };

        class SpriteLookupTable
        {
            BodyIndex mIndex;
            
        public:
            explicit SpriteLookupTable(const BodyIndex &index);
            virtual ~SpriteLookupTable();

            size_t Get(const std::string &group, float frame, const core::Direction &dir) const;
        };
    }
}

#endif  // BODYINDEX_H_

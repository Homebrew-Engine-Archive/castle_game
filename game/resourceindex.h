#ifndef RESOURCEINDEX_H_
#define RESOURCEINDEX_H_

#include <map>
#include <string>
#include <vector>

#include <game/direction.h>

namespace castle
{
    namespace gfx
    {
        using DirSet = std::vector<core::Direction>;
        
        struct ResourceGroup
        {
            std::string name;
            size_t size;
            DirSet dirs;
        };    
        
        class IndexTable
        {
            std::map<core::Direction, std::vector<size_t>> mIndex;
            
        public:
            void AddIndex(const core::Direction &dir, size_t index);
        };
        
        class ResourceIndex
        {
            std::map<std::string, IndexTable> mTables;
            
        public:
            explicit ResourceIndex(const std::vector<ResourceGroup> &groups);
            const IndexTable& QueryIndex(const std::string &group) const;
        };

    }
}

#endif  // RESOURCEINDEX_H_

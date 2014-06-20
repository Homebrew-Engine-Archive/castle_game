#ifndef RESOURCEPACK_H_
#define RESOURCEPACK_H_

#include <string>
#include <map>
#include <tuple>
#include <vector>

#include <game/collection.h>
#include <game/direction.h>
#include <game/image.h>

namespace castle
{
    namespace gfx
    {
        using DirectionSet = std::vector<core::Direction>;

        struct SpriteGroup
        {
            std::string name;
            size_t size;
            DirectionSet dirs;
        };    
        
        class SpriteSequence
        {
            size_t mCount;
            size_t mDirs;
            std::vector<castle::Image> images;
        public:
            SpriteSequence(size_t count, size_t dirs);
            castle::Image& GetImage(const core::Direction &dir, float index);
        };
        
        class ResourcePack
        {
            const Collection &mCollection;
            std::vector<SpriteGroup> mGroups;
            
        public:
            explicit ResourcePack(const Collection &collection);
            virtual ~ResourcePack();

            void AddGroup(const SpriteGroup &group);
            
            const SpriteSequence ReadSpriteSequence(const std::string &groupName);
        };
    }
}

#endif  // RESOURCEPACK_H_

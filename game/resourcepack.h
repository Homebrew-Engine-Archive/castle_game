#ifndef RESOURCEPACK_H_
#define RESOURCEPACK_H_

#include <string>
#include <vector>

#include <game/collection.h>
#include <game/direction.h>
#include <game/image.h>

namespace castle
{
    namespace gfx
    {
        using DirSet = std::vector<core::Direction>;
        
        struct SpriteGroup
        {
            std::string name;
            size_t size;
            DirSet dirs;
        };    
        
        struct SpriteSequence
        {
            castle::Image GetImage(const core::Direction &dir, float index);
        };
        
        class ResourcePack
        {
            const Collection &mCollection;
            std::vector<SpriteGroup> mGroups;
            
        public:
            explicit ResourcePack(const Collection &collection);
            virtual ~ResourcePack();

            void AddGroup(const SpriteGroup &group);
            const SpriteSequence ReadGroup(const std::string &groupName);
        };
    }
}

#endif  // RESOURCEPACK_H_

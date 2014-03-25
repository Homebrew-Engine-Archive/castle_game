#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include "direction.h"
#include "gm1.h"

namespace Render
{

    class ResourceManager
    {
    public:
        void AddSprite(const std::string &archive, size_t index, const std::string &group, const Direction &dir, size_t seq);
    };
   
}

#endif

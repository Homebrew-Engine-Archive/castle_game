#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include <game/filesystem.h>
#include <game/collection.h>

namespace castle
{
    namespace gfx
    {
        class GraphicsManager
        {
            
        public:
            GraphicsManager();
            virtual ~GraphicsManager();

            void AddCollectionPath(const fs::path &path);
            void LoadAll();
        };
    }
}

#endif // GRAPHICSMANAGER_H_

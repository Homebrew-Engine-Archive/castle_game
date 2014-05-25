#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include <SDL.h>

#include <string>
#include <unordered_map>

#include <game/surface.h>
#include <game/filesystem.h>
#include <game/point.h>
#include <game/sdl_utils.h>

namespace Render
{
    class Renderer;
}

namespace Graphics
{
    class GraphicsManager
    {
        std::vector<struct TileData> mTiles;
    public:
        GraphicsManager();
        virtual ~GraphicsManager();
        
        int CreateTile(const std::string &gmpath, int entry);
    };
}

#endif // GRAPHICSMANAGER_H_

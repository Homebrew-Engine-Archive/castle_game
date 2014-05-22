#include "graphicsmanager.h"

#include <game/gm1palette.h>
#include <game/gm1entryreader.h>
#include <game/gm1reader.h>

#include <boost/filesystem.hpp>

namespace Graphics
{
    struct TileData
    {
        Surface face;
        Point center;
        TileData *flat;
    };
    
    GraphicsManager::GraphicsManager()
        : mTiles()
    {
    }
    
    GraphicsManager::~GraphicsManager() = default;
    
    int GraphicsManager::CreateTile(const std::string &gmname, int entry)
    {
        return 0;
    }
}

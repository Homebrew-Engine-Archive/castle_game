#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include <SDL.h>

#include <string>
#include <unordered_map>

#include <game/landscape.h>
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
    class Picture
    {
        Surface mSurface;
        Point mAnchor;
        
    public:
        
    };

    struct TileSet
    {
        std::vector<Picture> pictures;
    };

    class GraphicsManager
    {
    public:
        GraphicsManager();
        virtual ~GraphicsManager();
        
        Picture const& GetPicture(const std::string &name) const;
        
    };
}

#endif // GRAPHICSMANAGER_H_

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
#include <game/gm1palette.h>

namespace Render
{
    class Renderer;
}

namespace Graphics
{
    class GraphicsManager
    {
    public:
        GraphicsManager();
        virtual ~GraphicsManager();
        
        Picture const& GetSurface(const std::string &name) const;
        Picture const& GetTile(const std::string &name) const;
        GM1::Palette const& GetPalette(const std::string &name) const;
    };
}

#endif // GRAPHICSMANAGER_H_

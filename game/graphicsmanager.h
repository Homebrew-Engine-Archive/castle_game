#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include <SDL.h>

#include <string>
#include <unordered_map>

#include <game/surface.h>
#include <game/filesystem.h>
#include <game/point.h>
#include <game/sdl_utils.h>

namespace Graphics
{
    struct Image
    {
        virtual void Draw(Surface &surface) {};
    };

    struct SpritePaletteSet
    {
    
    };

    struct TiledImage : public Image
    {
        const Surface &flatRef;
        const Surface &surfaceRef;
    };

    struct TranslucentImage : public Image
    {
        const Surface &alphaRef;
        const Surface &surfaceRect;
    };

    struct SpriteImage : public Image
    {
        const SpritePaletteSet &paletteSetRef;
        const Surface &surfaceRef;
    };
    
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

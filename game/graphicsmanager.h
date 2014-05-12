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
    class GraphicsManager
    {
        std::unordered_map<std::string, Surface> mGfxSurfaces;

        std::unordered_map<std::string, std::vector<PalettePtr>> mGm1Palettes;
        
        std::unordered_map<std::string, std::vector<Surface>> mGm1Surfaces;
        std::unordered_map<std::string, std::vector<Point>> mGm1Centers;
        
    public:
        GraphicsManager();

        void CacheGM1(const std::string &gmname);

        int CreateTile(const std::string &gmpath, int entry);
        
        void SetFlatTile(int tile, int flat);
        Surface GetFlatTile(int tile);
        Surface GetTileSurface(int tile);
        Point GetTileCenter(int tile);

        int CreateSprite(const std::string &gmpath, int entry);
        SDL_Palette* GetSpritePalette(int sprite, int palette);
        Surface GetSpriteSurface(int sprite);
        Point GetSpriteCenter(int sprite);
        
        Surface Gfx(const std::string &gfxpath, bool cache = true);
        Surface GetImage(const std::string &gmpath, int entry);
        SDL_Palette* GetPalette(const std::string &gmpath, int palette);
        Point GetImageCenter(const std::string &gmpath, int entry);
    };
}

#endif // GRAPHICSMANAGER_H_

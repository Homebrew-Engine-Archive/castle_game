#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>

#include <SDL.h>

#include <game/collection.h>
#include <game/surface.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>

class CollectionData;
class Surface;

namespace Render
{
    class Renderer
    {
        SDL_Renderer *mRenderer;
        int mScreenWidth;
        int mScreenHeight;
        int mScreenFormat;
        TexturePtr mScreenTexture;
        Surface mScreenSurface;
        std::map<fs::path, Surface> mGFXCache;
        std::map<fs::path, CollectionDataPtr> mGMCache;

        bool ReallocationRequired(int width, int heigth);
        bool CreateScreenTexture(int width, int height);
        bool CreateScreenSurface(void *pixels, int width, int height, int pitch);

    public:
        Renderer(SDL_Renderer *renderer);
        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        virtual ~Renderer() = default;
        
        Surface BeginFrame();
        void EndFrame();
        SDL_Rect GetOutputSize() const;
        void SetWindowSize(int width, int height);
        void AdjustBufferSize(int width, int height);
        Surface QuerySurface(const fs::path &filename);
        const CollectionData &QueryCollection(const fs::path &filename);
        bool CacheCollection(const fs::path &filepath);
    };
    
} // namespace Render

#endif

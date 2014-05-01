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
#include <game/texturelocker.h>

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
        bool mScreenClear;
        TexturePtr mScreenTexture;
        Surface mScreenSurface;
        std::unique_ptr<TextureLocker> mLock;
        std::map<fs::path, Surface> mGFXCache;
        std::map<fs::path, CollectionDataPtr> mGMCache;

        bool ReallocationRequired(int width, int heigth);
        void CreateScreenTexture(int width, int height);
        void CreateScreenSurface(void *pixels, int pitch);

    public:
        Renderer(SDL_Renderer *renderer);
        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        
        std::vector<uint32_t> AvailablePixelFormats() const;

        bool ValidTextureSize(int width, int height) const;
        
        Surface BeginFrame();
        void EndFrame();
        SDL_Rect GetOutputSize() const;
        void SetWindowSize(int width, int height);
        void AdjustBufferSize(int width, int height);
        Surface QuerySurface(const fs::path &filename);
        CollectionData const& QueryCollection(const fs::path &filename);
        bool CacheCollection(const fs::path &filepath);
        
        void EnableClearScreen(bool on);
    };
    
} // namespace Render

#endif

#include "graphicsmanager.h"

#include <map>
#include <unordered_map>
#include <unordered_set>

#include "loadingscreen.h"
#include "filesystem.h"
#include "renderer.h"

namespace Render
{

    GraphicsManager::GraphicsManager(Renderer *renderer)
        : mRenderer(renderer)
        , mPaletteIndex(0)
        , mIds()
        , mReaders()
        , mSurfaces()
    {   
    }
    
    void GraphicsManager::LoadAll(UI::LoadingScreen *loadingScreen)
    {
        if(mIds.size() == 0)
            throw std::runtime_error("Empty load queue");
        
        loadingScreen->SetProgressMax(mIds.size());
        
        int done = 0;
        uint32_t time = SDL_GetTicks();
        for(GraphicsID const& id : mIds) {
            loadingScreen->SetProgressDone(done);
            Load(id);
            ++done;
            uint32_t currentTime = SDL_GetTicks();
            if(loadingScreen->IsDirty(currentTime - time)) {
                Surface surface = mRenderer->BeginFrame();
                loadingScreen->Draw(surface);
                mRenderer->EndFrame();
            }
            time = currentTime;
        }
    }

    void GraphicsManager::Load(GraphicsID const& id)
    {
        GM::GM1Reader const& reader = GetReader(id);
    }

    void GraphicsManager::SetPaletteIndex(size_t index)
    {
        mPaletteIndex = 0;
    }
    
    void GraphicsManager::PaintGraphics(GraphicsID const& id)
    {
        
    }
    
    GM::GM1Reader const& GraphicsManager::GetReader(GraphicsID const& id)
    {
    }
    
    GraphicsID GraphicsManager::AddGraphics(std::string path, size_t index)
    {
        GraphicsID id(std::move(path), index);
        mIds.insert(id);
        return id;
    }

    void GraphicsManager::RemoveGraphics(GraphicsID const& id)
    {
        auto iter = mIds.find(id);
        if(iter != mIds.end()) {
            mIds.erase(iter);
        }
    }
    
}

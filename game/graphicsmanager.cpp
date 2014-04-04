#include "graphicsmanager.h"

#include <map>
#include <unordered_map>
#include <unordered_set>

#include <game/loadingscreen.h>
#include <game/filesystem.h>
#include <game/renderer.h>

namespace Render
{

    GraphicsManager::GraphicsManager(Renderer *renderer)
        : mRenderer(renderer)
        , mPaletteIndex(0)
        , mUniqueGraphicsID(0)
        , mReaders()
        , mArchives()
        , mEntries()
        , mSurfaces()
    {   
    }
    
    void GraphicsManager::LoadAll(UI::LoadingScreen *loadingScreen)
    {
        
    }

    void GraphicsManager::Load(GraphicsID const& id)
    {
        GM1::GM1Reader const& reader = GetReader(id);
    }

    void GraphicsManager::SetPaletteIndex(size_t index)
    {
        mPaletteIndex = 0;
    }
    
    void GraphicsManager::PaintGraphics(GraphicsID const& id)
    {
        
    }    
    
    GraphicsID GraphicsManager::AddGraphics(std::string archive, size_t index)
    {
        return mUniqueGraphicsID++;
    }

    GraphicsID GraphicsManager::FindGraphics(const std::string &archive, size_t index)
    {
        
    }
    
    void GraphicsManager::RemoveGraphics(GraphicsID const& id)
    {
        
    }

    GM1::GM1Reader const& GraphicsManager::GetReader(GraphicsID const& id)
    {
        
    }

    
}

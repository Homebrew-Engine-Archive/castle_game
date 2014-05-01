#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include <game/gm1palette.h>
#include <game/gm1.h>
#include <game/gm1reader.h>
#include <game/surface.h>
#include <unordered_map>
#include <string>

namespace GM1
{
    class Header;
    class EntryHeader;
}

namespace Render
{
    class CollectionData;
    
    class GraphicsManager
    {
        std::vector<PalettePtr> mPalettes;
        std::vector<Surface> mSurfaces;
        std::vector<EntryHeader> mEntryHeaders;
        std::vector<Header> mHeaders;
        
    public:
        GraphicsManager();
        GraphicsManager(GraphicsManager const&);
        GraphicsManager& operator=(GraphicsManager const&);
        virtual ~GraphicsManager();

        SDL_Palette const* GetPalette(size_t target) const;
        GM1::EntryHeader const& GetEntryHeader(size_t target) const;
        GM1::Header const& GetHeader(size_t target) const;
        Surface& GetSurface(size_t target);
    };
}

#endif // GRAPHICSMANAGER_H_

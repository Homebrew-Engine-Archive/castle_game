#include "atlas.h"
#include <game/geometry.h>
#include <algorithm>
#include <iostream>

namespace
{

    struct SurfaceCompare
    {
        bool operator()(const Surface &lhs, const Surface &rhs) const {
            return (lhs->h < rhs->h) || ((lhs->h == rhs->h) && (lhs->w < rhs->w));
        }
    };

    template<class First, class Second, class Compare>
    struct compare_first
    {
        bool operator()(const std::pair<First, Second> &lhs,
                        const std::pair<First, Second> &rhs) const {
            return Compare()(lhs.first, rhs.first);
        }
    };

    typedef compare_first<Surface, size_t, SurfaceCompare> CompareHeight;

}

namespace Render
{

    bool GetAtlasPartition(const std::vector<Surface> &surfaces, std::vector<SDL_Rect> &partition, int maxWidth, int maxHeight)
    {
        partition.resize(surfaces.size());

        // for keeping original index
        std::vector<std::pair<Surface, size_t>> indexed;
    
        for(size_t index = 0; index < surfaces.size(); ++index) {
            Surface surface = surfaces.at(index);
            if(!surface.Null())
                indexed.emplace_back(surface, index);
        }

        // Descending by surface pixels count
        std::sort(indexed.rbegin(), indexed.rend(), CompareHeight());
    
        int x = 0;
        int y = 0;
        int maxHeightLastRow = 0;

        for(const std::pair<Surface, size_t> &ixed : indexed) {
            Surface surface = ixed.first;
            size_t origIndex = ixed.second;

            int w = surface->w;
            int h = surface->h;

            // new line
            if(x + w > maxWidth) {
                if(y + maxHeightLastRow > maxHeight) {
                    return false;
                }
                y += maxHeightLastRow;
                maxHeightLastRow = 0;
                x = 0;
            }
        
            SDL_Rect rect = MakeRect(x, y, w, h);
            x += w;

            maxHeightLastRow = std::max(maxHeightLastRow, h);

            if(y + maxHeightLastRow > maxHeight) {
                return false;
            }
        
            partition.at(origIndex) = rect;
        }

        return true;
    }

    Surface MakeSurfaceAtlas(const std::vector<Surface> &surfaces, std::vector<SDL_Rect> &partition, int maxWidth, int maxHeight)
    {
        GetAtlasPartition(surfaces, partition, maxWidth, maxHeight);
        return BuildAtlas(surfaces, partition);
    }

    Surface BuildAtlas(const std::vector<Surface> &surfaces, const std::vector<SDL_Rect> &partition)
    {
        if(surfaces.empty())
            return Surface();

        int boundWidth = 0;
        int boundHeight = 0;
        for(const SDL_Rect &rect : partition) {
            boundWidth = std::max(rect.x + rect.w, boundWidth);
            boundHeight = std::max(rect.y + rect.h, boundHeight);
        }

        Surface ref;
        for(const Surface &surface : surfaces) {
            if(!surface.Null())
                ref = surface;
        }

        if(ref.Null())
            return Surface();

        Surface atlas = CopySurfaceFormat(ref, boundWidth, boundHeight);
        if(atlas.Null()) {
            std::cerr << "BuildAtlas failed: "
                      << SDL_GetError()
                      << std::endl;
            return Surface();
        }

        for(size_t i = 0; i < surfaces.size(); ++i) {
            SDL_Rect bounds = partition.at(i);
            Surface surface = surfaces.at(i);
            if(!surface.Null())
                BlitSurface(surface, NULL, atlas, &bounds);
        }
    
        return atlas;
    }

} // namespace Render

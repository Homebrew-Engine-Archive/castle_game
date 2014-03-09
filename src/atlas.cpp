#include "atlas.h"

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

Surface MakeSurfaceAtlas(const std::vector<Surface> &surfaces, std::vector<SDL_Rect> &partition, int maxWidth, int maxHeight)
{
    partition.resize(surfaces.size());

    // for keeping original index
    std::vector<std::pair<Surface, size_t>> indexed;
    
    for(size_t index = 0; index < surfaces.size(); ++index) {
        indexed.emplace_back(surfaces.at(index), index);
    }

    // Descending by surface pixels count
    std::sort(indexed.rbegin(), indexed.rend(), CompareHeight());
    
    int x = 0;
    int y = 0;
    int maxHeightLastRow = 0;

    for(size_t i = 0; i < indexed.size(); ++i) {
        Surface surface = indexed.at(i).first;
        size_t origIndex = indexed.at(i).second;

        int w = surface->w;
        int h = surface->h;

        // new line
        if(x + w > maxWidth) {
            y += maxHeightLastRow;
            maxHeightLastRow = 0;
            x = 0;
        }
        
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        x += w;

        maxHeightLastRow = std::max(maxHeightLastRow, h);

        if(y + maxHeightLastRow > maxHeight) {
            std::clog << "Max height reached" << std::endl;
            return Surface();
        }
        
        partition.at(origIndex) = rect;
    }

    return BuildAtlas(surfaces, partition);
}

Surface BuildAtlas(const std::vector<Surface> &surfaces, const std::vector<SDL_Rect> &partition)
{
    if(surfaces.empty())
        return Surface();

    int boundWidth = 0;
    int boundHeight = 0;
    for(const auto &rect : partition) {
        if(rect.x + rect.w > boundWidth)
            boundWidth = rect.x + rect.w;
        if(rect.y + rect.h > boundHeight)
            boundHeight = rect.y + rect.h;
    }

    Surface atlas = CopySurfaceFormat(surfaces.front(), boundWidth, boundHeight);
    if(atlas.Null())
        return Surface();

    for(size_t i = 0; i < surfaces.size(); ++i) {
        SDL_Rect bounds = partition.at(i);
        BlitSurface(surfaces.at(i), NULL, atlas, &bounds);
    }
    
    return atlas;
}

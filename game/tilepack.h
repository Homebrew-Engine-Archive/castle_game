#ifndef TILEPACK_H_
#define TILEPACK_H_

#include <game/surface.h>
#include <vector>

namespace GM1
{
    class GM1Reader;
}

namespace Graphics
{
    class TileSurface
    {
        Surface mSurface;
        
    public:
        explicit TileSurface(Surface surface);
        Surface& Get()
            {return mSurface;}

        Surface const& Get() const
            {return mSurface;}
    };
    
    class TilePack
    {
        std::vector<TileSurface> mTiles;
    
    public:
        explicit TilePack(size_t count);
        
        TileSurface& Get(int index)
            {return mTiles.at(index);}
        
        TileSurface const& Get(int index) const
            {return mTiles.at(index);}

        size_t Size() const
            {return mTiles.size();}
    };

    class TilePackReader
    {
    public:
        explicit TilePackReader(const GM1::GM1Reader &reader);
        TilePack Next();
    };
}

#endif // TILEPACK_H_

#ifndef SPRITEPACK_H_
#define SPRITEPACK_H_

#include <vector>

#include <game/point.h>
#include <game/surface.h>
#include <game/gm1palette.h>
#include <game/collection.h>

namespace GM1
{
    class GM1Reader;
}

namespace Graphics
{
    class SpriteSurface
    {
        Surface mSurface;
        Point mAnchor;
        
    public:
        explicit SpriteSurface(Surface surface);
        Surface& Get();
        Surface const& Get() const;

        const Point Anchor() const;
    };

    class PalettePack
    {
        std::vector<GM1::Palette> mPalettes;

    public:
        explicit PalettePack(size_t size);

        GM1::Palette& Get(PaletteName name);
        GM1::Palette const& Get(PaletteName name) const;
    };

    template<class>
    class SpritePack
    {
        std::vector<SpriteSurface> mSprites;
        
    public:
        explicit SpritePack(size_t size);
        
        SpriteSurface& Get(size_t index);
        SpriteSurface const& Get(size_t index) const;
    };

    class SpritePackReader
    {
    public:
        explicit SpritePackReader(const GM1::Reader &reader);
        SpritePack Next();
    };
}

#endif // SPRITEPACK_H_

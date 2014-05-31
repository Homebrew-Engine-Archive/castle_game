#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>

#include <game/surface.h>
#include <game/gm1.h>
#include <game/filesystem.h>
#include <game/sdl_utils.h>

class Point;

namespace GM1
{
    class GM1Reader;
    class Palette;
}

namespace Castle
{
    enum class PaletteName : size_t
    {
        Unknown0     = 0,
        Blue         = 1,
        Red          = 2,
        Orange       = 3,
        Yellow       = 4,
        Purple       = 5,
        Black        = 6,
        Cyan         = 7,
        Green        = 8,
        Unknown      = 9
    };

    class Collection
    {
    public:
        explicit Collection(const GM1::GM1Reader &reader);
        Collection(const Collection &collection);
        Collection& operator=(const Collection &collection);

        int Count() const;
        GM1::Header const& GetHeader() const;
        const Point Anchor() const;
        const Surface GetSurface(size_t index) const;
        GM1::EntryHeader const& GetEntryHeader(size_t index) const;        
        GM1::Palette const& GetPalette(PaletteName name) const;

    protected:
        GM1::Header mHeader;
        std::vector<GM1::Palette> mPalettes;
        std::vector<Surface> mEntries;
        std::vector<GM1::EntryHeader> mHeaders;
    };

    Surface LoadTGX(const fs::path &filename);
    Collection LoadGM1(const fs::path &filename);
}

#endif

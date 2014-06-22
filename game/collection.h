#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <cstddef>
#include <vector>

#include <SDL.h>

#include <game/image.h>
#include <game/gm1.h>
#include <game/filesystem.h>
#include <game/sdl_utils.h>
#include <game/palettename.h>

namespace core
{
    class Point;
}

namespace gm1
{
    class GM1Reader;
    class Palette;
}

namespace castle
{
    namespace gfx
    {
        class Collection
        {
        public:
            explicit Collection(const gm1::GM1Reader &reader);
            Collection(const Collection &collection);
            Collection& operator=(const Collection &collection);
            virtual ~Collection();

            size_t Count() const;
            const gm1::Header& GetHeader() const;
            const core::Point Anchor() const;
            const castle::Image& GetImage(size_t index) const;
            const gm1::EntryHeader& GetEntryHeader(size_t index) const;        
            const castle::Palette& GetPalette(const PaletteName &name) const;

        private:
            gm1::Header mHeader;
            std::vector<castle::Palette> mPalettes;
            std::vector<castle::Image> mEntries;
            std::vector<gm1::EntryHeader> mHeaders;
        };

        Image LoadTGX(const vfs::path &filename);
        Collection LoadGM1(const vfs::path &filename);
    }
}

#endif

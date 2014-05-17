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

struct CollectionEntry
{
    GM1::EntryHeader header;
    Surface surface;
};

typedef std::unique_ptr<CollectionEntry> CollectionEntryPtr;

struct CollectionData
{
    GM1::Header header;
    std::vector<CollectionEntry> entries;
    std::vector<PalettePtr> palettes;
};

typedef std::unique_ptr<CollectionData> CollectionDataPtr;

Surface LoadTGX(const fs::path &filename);
CollectionData LoadGM1(const fs::path &filename);

#endif

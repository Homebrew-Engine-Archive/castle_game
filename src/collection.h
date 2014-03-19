#ifndef COLLECTION_H_
#define COLLECTION_H_

#include "SDL.h"
#include <string>
#include <vector>
#include <memory>
#include "surface.h"
#include "gm1.h"
#include "filesystem.h"

struct CollectionAtlas
{
    gm1::Collection gm1;
    Surface map;
    CollectionAtlas(SDL_RWops *src);
};

typedef std::unique_ptr<CollectionAtlas> CollectionAtlasPtr;

struct CollectionEntry
{
    gm1::ImageHeader header;
    Surface surface;
    CollectionEntry(const gm1::ImageHeader &hdr_, const Surface &sf_);
};

typedef std::unique_ptr<CollectionEntry> CollectionEntryPtr;

struct CollectionData
{
    gm1::Header header;
    std::vector<CollectionEntry> entries;
    std::vector<PalettePtr> palettes;
};

typedef std::unique_ptr<CollectionData> CollectionDataPtr;

struct FontCollectionInfo
{
    FilePath filename;
    std::string name;
    std::vector<int> sizes;
    std::vector<int> alphabet;
};

typedef std::unique_ptr<FontCollectionInfo> FontCollectionInfoPtr;

Surface LoadSurface(const FilePath &filename);
CollectionDataPtr LoadCollectionData(const FilePath &filename);
CollectionAtlasPtr LoadCollectionAtlas(const FilePath &filename);

#endif

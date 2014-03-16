#include "collection.h"
#include <stdexcept>
#include "rw.h"
#include "tgx.h"

CollectionAtlas::CollectionAtlas(SDL_RWops *src)
    : gm1(src)
    , map(gm1::LoadAtlas(src, gm1))
{ }

CollectionEntry::CollectionEntry(const gm1::ImageHeader &hdr_, const Surface &sf_)
    : header(hdr_)
    , surface(sf_)
{ }

CollectionDataPtr LoadCollectionData(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        
        if(!src)
            throw std::runtime_error("file not readable");
        
        gm1::Collection gm1(src.get());

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = gm1.header;
        
        std::vector<Surface> atlas;
        gm1::LoadEntries(src.get(), gm1, atlas);
        
        for(size_t n = 0; n < GM1_PALETTE_COUNT; ++n) {
            PalettePtr palette =
                PalettePtr(
                    gm1::CreateSDLPaletteFrom(gm1.palettes[n]));
            ptr->palettes.push_back(std::move(palette));
        }
        
        for(size_t n = 0; n < gm1.header.imageCount; ++n) {
            gm1::ImageHeader header = gm1.headers[n];
            Surface surface = atlas[n];
            ptr->entries.emplace_back(header, surface);
        }

        return ptr;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImageCollection: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        return CollectionDataPtr(nullptr);
    }
}

CollectionAtlasPtr LoadCollectionAtlas(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        if(!src)
            throw std::runtime_error("file not readable");
        
        CollectionAtlasPtr ptr(
            new CollectionAtlas(src.get()));

        return ptr;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadCollectionAtlas: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        return CollectionAtlasPtr(nullptr);
    }
}

Surface LoadSurface(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        
        if(!src)
            throw std::runtime_error("file not readable");
        
        return tgx::LoadStandaloneImage(src.get());
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImage: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        return Surface();
    }
}

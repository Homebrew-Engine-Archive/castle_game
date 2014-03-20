#include "collection.h"
#include <stdexcept>
#include "rw.h"
#include "tgx.h"

CollectionAtlas::CollectionAtlas(SDL_RWops *src)
    : gm1(src)
    , map(GM::LoadAtlas(src, gm1))
{ }

CollectionEntry::CollectionEntry(const GM::ImageHeader &hdr_, const Surface &sf_)
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
        
        GM::Collection gm1(src.get());

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = gm1.header;
        
        std::vector<Surface> atlas;
        GM::LoadEntries(src.get(), gm1, atlas);
        
        for(const GM::Palette &pal : gm1.palettes) {
            PalettePtr palette =
                PalettePtr(
                    GM::CreateSDLPaletteFrom(pal));
            ptr->palettes.push_back(std::move(palette));
        }
        
        for(size_t n = 0; n < gm1.size(); ++n) {
            GM::ImageHeader header = gm1.headers[n];
            Surface &surface = atlas[n];
            ptr->entries.emplace_back(header, surface);
        }

        return ptr;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImageCollection: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        throw;
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
        throw;
    }
}

Surface LoadSurface(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        
        if(!src)
            throw std::runtime_error("file not readable");
        
        return TGX::LoadStandaloneImage(src.get());
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImage: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        throw;
    }
}

namespace
{

    template<class T>
    std::ostream &operator<<(std::ostream &out, const std::vector<T> &xs)
    {
        out << '[';
        for(const T &x : xs) {
            out << ' ' << x;
        }
        out << ']';
        return out;
    }
    
}

std::ostream &operator<<(std::ostream &out, const FontCollectionInfo &info)
{
    out << '['
        << info.filename << "; "
        << info.name << "; "
        << info.sizes << "; "
        << info.alphabet;
    
    out << ']';
    return out;
}

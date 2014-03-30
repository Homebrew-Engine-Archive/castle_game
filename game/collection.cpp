#include "collection.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "rw.h"
#include "sdl_utils.h"
#include "tgx.h"

CollectionAtlas::CollectionAtlas(SDL_RWops *src)
    : gm1(src)
    , map(GM::LoadAtlas(src, gm1))
{ }

CollectionEntry::CollectionEntry(const GM::ImageHeader &hdr_, const Surface &sf_)
    : header(hdr_)
    , surface(sf_)
{ }

namespace
{

    void Fail(const std::string &where)
    {
        std::ostringstream oss;
        oss << where << " failed: " << SDL_GetError();
        throw std::runtime_error(oss.str());
    }
    
    PalettePtr ConvertPaletteToSDLPalette(const GM::Palette &palette)
    {
        PalettePtr ptr(SDL_AllocPalette(GM::CollectionPaletteColors));
        if(!ptr) {
            Fail("SDL_AllocPalette");
        }

        std::vector<SDL_Color> colors;
        colors.reserve(ptr->ncolors);
        for(auto color : palette) {
            colors.push_back(
                MakeColor(
                    TGX::GetRed(color),
                    TGX::GetGreen(color),
                    TGX::GetBlue(color),
                    TGX::GetAlpha(color)));
        }

        if(SDL_SetPaletteColors(ptr.get(), &colors[0], 0, ptr->ncolors) < 0) {
            Fail("SDL_SetPaletteColors");
        }
    
        return ptr;
    }
}

CollectionDataPtr LoadCollectionData(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr &&src = RWFromFileBuffer(filebuff);
        
        if(!src)
            throw std::runtime_error("file not readable");
        
        GM::Collection gm1(src.get());

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = gm1.header;
        for(const GM::Palette &palette : gm1.palettes) {
            ptr->palettes.push_back(
                std::move(
                    ConvertPaletteToSDLPalette(palette)));
        }
        
        std::vector<Surface> atlas;
        GM::LoadEntries(src.get(), gm1, atlas);
        for(size_t n = 0; n < gm1.size(); ++n) {
            GM::ImageHeader header = gm1.headers[n];
            Surface &surface = atlas[n];
            ptr->entries.emplace_back(header, surface);
        }
        
        return ptr;
    } catch(const std::exception &e) {
        std::ostringstream oss;
        oss << "In LoadImageCollection [with filename = " << filename << ']' << std::endl;
        oss << e.what() << std::endl;
        throw std::runtime_error(oss.str());
    }
}

CollectionAtlasPtr LoadCollectionAtlas(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr &&src = RWFromFileBuffer(filebuff);
        if(!src)
            throw std::runtime_error("file not readable");
        
        CollectionAtlasPtr ptr(new CollectionAtlas(src.get()));

        return ptr;
    } catch(const std::exception &e) {
        std::ostringstream oss;
        oss << "In LoadCollectionAtlas [with filename = " << filename << ']' << std::endl;
        oss << e.what() << std::endl;
        throw std::runtime_error(oss.str());
    }
}

Surface LoadSurface(const FilePath &filename)
{
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr &&src = RWFromFileBuffer(filebuff);
        
        if(!src)
            throw std::runtime_error("file not readable");
        
        return TGX::LoadStandaloneImage(src.get());
        
    } catch(const std::exception &e) {
        std::ostringstream oss;
        oss << "In LoadSurface [with filename = " << filename << ']' << std::endl;
        oss << e.what() << std::endl;
        throw std::runtime_error(oss.str());
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

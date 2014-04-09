#include "collection.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem/fstream.hpp>
#include <boost/current_function.hpp>

#include <game/gm1entryreader.h>
#include <game/gm1reader.h>
#include <game/sdl_utils.h>
#include <game/tgx.h>

CollectionEntry::CollectionEntry(const GM1::EntryHeader &hdr_, const Surface &sf_)
    : header(hdr_)
    , surface(sf_)
{ }

namespace
{
    
    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }

    SDL_PixelFormat const* GetPixelFormat()
    {
        static PixelFormatPtr ptr(SDL_AllocFormat(TGX::GetPixelFormatEnum()));
        return ptr.get();
    }
    
    PalettePtr GetSDLPalette(const GM1::Palette &palette)
    {
        PalettePtr ptr(SDL_AllocPalette(GM1::CollectionPaletteColors));
        if(!ptr) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
                
        std::vector<SDL_Color> colors;
        colors.reserve(ptr->ncolors);
        for(auto color : palette) {
            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 0;
            uint8_t alpha = 0;
            SDL_GetRGBA(color, GetPixelFormat(), &red, &green, &blue, &alpha);
            
            colors.push_back(
                MakeColor(red, green, blue, alpha));
        }

        if(SDL_SetPaletteColors(ptr.get(), &colors[0], 0, ptr->ncolors) < 0) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
    
        return ptr;
    }
}

CollectionDataPtr LoadCollectionData(const FilePath &path)
{
    try {
        GM1::GM1Reader reader;
        reader.Open(path);

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = reader.Header();
        for(int index = 0; index < reader.NumPalettes(); ++index) {
            const GM1::Palette &palette = reader.Palette(index);
            ptr->palettes.push_back(
                std::move(
                    GetSDLPalette(palette)));
        }

        for(int index = 0; index < reader.NumEntries(); ++index) {
            const GM1::EntryHeader &header = reader.EntryHeader(index);
            Surface entry = reader.Decode(index);
            ptr->entries.emplace_back(header, std::move(entry));
        }
        
        return ptr;
    } catch(const std::exception &e) {
        std::ostringstream oss;
        oss << "In LoadImageCollection [with filename = " << path << ']' << std::endl;
        oss << e.what() << std::endl;
        throw std::runtime_error(oss.str());
    }
}

Surface LoadSurface(const FilePath &path)
{
    using namespace boost;
    
    try {
        filesystem::ifstream fin(path, std::ios_base::binary);
        if(!fin.is_open()) {
            Fail(BOOST_CURRENT_FUNCTION, "Can't open file");
        }
        return TGX::ReadTGX(fin);
        
    } catch(const std::exception &e) {
        std::ostringstream oss;
        oss << "In LoadSurface [with filename = " << path << ']' << std::endl;
        oss << e.what() << std::endl;
        throw std::runtime_error(oss.str());
    }
}

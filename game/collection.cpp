#include "collection.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem/fstream.hpp>
#include <boost/current_function.hpp>

#include "gm1entryreader.h"
#include "gm1reader.h"
#include "rw.h"
#include "sdl_utils.h"
#include "tgx.h"

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
    
    PalettePtr ConvertPaletteToSDLPalette(const GM1::Palette &palette)
    {
        PalettePtr ptr(SDL_AllocPalette(GM1::CollectionPaletteColors));
        if(!ptr) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
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
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
    
        return ptr;
    }
}

CollectionDataPtr LoadCollectionData(const FilePath &path)
{
    try {
        GM1::GM1Reader reader(path);

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = reader.Header();
        for(int index = 0; index < reader.NumPalettes(); ++index) {
            const GM1::Palette &palette = reader.Palette(index);
            ptr->palettes.push_back(
                std::move(
                    ConvertPaletteToSDLPalette(palette)));
        }

        auto entryReader = GM1::CreateEntryReader(reader.Header());
        for(int index = 0; index < reader.NumEntries(); ++index) {
            const GM1::EntryHeader &header = reader.EntryHeader(index);
            Surface entry = entryReader->Load(reader, index);
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
        return TGX::LoadStandaloneImage(fin);
        
    } catch(const std::exception &e) {
        std::ostringstream oss;
        oss << "In LoadSurface [with filename = " << path << ']' << std::endl;
        oss << e.what() << std::endl;
        throw std::runtime_error(oss.str());
    }
}

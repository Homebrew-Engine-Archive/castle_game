#include "collection.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem/fstream.hpp>
#include <boost/current_function.hpp>

#include <game/gm1palette.h>
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

}
    
CollectionDataPtr LoadCollectionData(const FilePath &path)
{
    try {
        GM1::GM1Reader reader;
        reader.Open(path, GM1::GM1Reader::Cached);

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = reader.Header();
        
        for(int i = 0; i < reader.NumPalettes(); ++i) {
            ptr->palettes.push_back(
                std::move(
                    GM1::CreateSDLPalette(
                        reader.Palette(i))));
        }

        GM1::GM1EntryReader &entryReader = reader.EntryReader();
        for(int i = 0; i < reader.NumEntries(); ++i) {
            Surface entry = entryReader.Load(reader, i);
            const GM1::EntryHeader &header = reader.EntryHeader(i);
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

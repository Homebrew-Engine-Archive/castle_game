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

CollectionDataPtr LoadCollectionData(const fs::path &path)
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
            ptr->entries.push_back(
                CollectionEntry {
                    reader.EntryHeader(i),
                    entryReader.Load(reader, i)}
            );
        }
        
        return ptr;
    } catch(const std::exception &e) {
        std::cerr << "Error while load " << path << std::endl;
        throw;
    }
}

Surface LoadSurface(const fs::path &path)
{
    using namespace boost;
    
    try {
        filesystem::ifstream fin(path, std::ios_base::binary);
        if(!fin.is_open()) {
            std::ostringstream oss;
            oss << "Unable to read " << path << ": " << strerror(errno);
            throw std::runtime_error(oss.str());
        }
        return TGX::ReadTGX(fin);
        
    } catch(const std::exception &e) {
        std::cerr << "Error while load " << path << std::endl;
        throw;
    }
}

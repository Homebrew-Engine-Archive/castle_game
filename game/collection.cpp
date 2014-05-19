#include "collection.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <chrono>

#include <boost/filesystem/fstream.hpp>

#include <game/gm1palette.h>
#include <game/gm1reader.h>
#include <game/tgx.h>

CollectionData LoadGM1(const fs::path &path)
{
    std::chrono::steady_clock::time_point startAt = std::chrono::steady_clock::now();

    CollectionData data;
    GM1::GM1Reader gm1;

    try {
        gm1.Open(path, GM1::GM1Reader::Cached);
    } catch(const std::exception &error) {
        std::cerr << "open collection failed: " << error.what() << std::endl;
        throw;
    }
    
    data.header = gm1.Header();
    data.palettes.resize(gm1.NumPalettes());

    for(int i = 0; i < gm1.NumPalettes(); ++i) {
        try {
            data.palettes[i] = GM1::CreateSDLPalette(gm1.Palette(i));
        } catch(const std::exception &error) {
            std::cerr << "read palette failed: " << error.what() << std::endl;
            throw;
        }
    }

    data.entries.resize(gm1.NumEntries());
    for(int i = 0; i < gm1.NumEntries(); ++i) {
        data.entries[i].header = gm1.EntryHeader(i);
        try {
            data.entries[i].surface = gm1.ReadEntry(i);
        } catch(const std::exception &error) {
            std::cerr << "read entry failed: " << error.what() << std::endl;
            throw;
        }
    }

    std::chrono::steady_clock::time_point endAt = std::chrono::steady_clock::now();
    std::clog << "Load " << path << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(endAt - startAt).count() << "ms" << std::endl;
    
    return data;
}

Surface LoadTGX(const fs::path &path)
{
    boost::filesystem::ifstream fin(path, std::ios_base::binary);
    if(!fin.is_open()) {
        std::ostringstream oss;
        oss << "Unable to read " << path << ": " << strerror(errno);
        throw std::runtime_error(oss.str());
    }
    
    try {
        return TGX::ReadTGX(fin);
    } catch(const std::exception &error) {
        std::cerr << "read image failed: " << error.what() << std::endl;
        throw;
    }
}

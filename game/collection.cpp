#include "collection.h"

#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>

#include <boost/filesystem/fstream.hpp>

#include <game/gm1palette.h>
#include <game/gm1reader.h>
#include <game/tgx.h>

Collection LoadGM1(const fs::path &path)
{
    GM1::GM1Reader gm1;
    try {
        gm1.Open(path, GM1::GM1Reader::Cached);
    } catch(const std::exception &error) {
        std::cerr << "open collection failed: " << error.what() << std::endl;
        throw;
    }

    return Collection(gm1);
    
    // using namespace std::chrono;
    // steady_clock::time_point startAt = steady_clock::now();

    // Collection data;
    // GM1::GM1Reader gm1;

    // try {
    //     gm1.Open(path, GM1::GM1Reader::Cached);
    // } catch(const std::exception &error) {
    //     std::cerr << "open collection failed: " << error.what() << std::endl;
    //     throw;
    // }
    
    // data.header = gm1.Header();
    // data.palettes.resize(gm1.NumPalettes());

    // for(int i = 0; i < gm1.NumPalettes(); ++i) {
    //     try {
    //         data.palettes[i] = gm1.Palette(i);
    //     } catch(const std::exception &error) {
    //         std::cerr << "read palette failed: " << error.what() << std::endl;
    //         throw;
    //     }
    // }

    // data.entries.resize(gm1.NumEntries());
    // for(int i = 0; i < gm1.NumEntries(); ++i) {
    //     data.entries[i].header = gm1.EntryHeader(i);
    //     try {
    //         data.entries[i].surface = gm1.ReadEntry(i);
    //     } catch(const std::exception &error) {
    //         std::cerr << "read entry failed: " << error.what() << std::endl;
    //         throw;
    //     }
    // }

    // steady_clock::time_point endAt = steady_clock::now();
    // std::clog << "Load " << path << ": " << duration_cast<milliseconds>(endAt - startAt).count() << "ms" << std::endl;
    
    // return data;
}

Surface LoadTGX(const fs::path &path)
{
    boost::filesystem::ifstream fin(path, std::ios_base::binary);

    if(!fin.is_open()) {
        std::cerr << "fail reading " << path << std::endl;
        throw std::runtime_error(strerror(errno));
    }
    
    try {
        return TGX::ReadTGX(fin);
    } catch(const std::exception &error) {
        std::cerr << "read image failed: " << error.what() << std::endl;
        throw;
    }
}

Collection::Collection(const Collection &collection) = default;

Collection& Collection::operator=(const Collection &collection) = default;

Collection::Collection(const GM1::GM1Reader &reader)
    : mHeader(reader.Header())
    , mPalettes(reader.NumPalettes())
    , mEntries(reader.NumEntries())
    , mHeaders(reader.NumEntries())
{
    for(int n = 0; n < reader.NumPalettes(); ++n) {
        mPalettes[n] = reader.Palette(n);
    }
    for(int n = 0; n < reader.NumEntries(); ++n) {
        try {
            mEntries[n] = reader.ReadEntry(n);
        } catch(const std::exception &error) {
            std::cerr << "read entry failed: what=" << error.what() << " entry=" << n << std::endl;
            throw;
        }
        mHeaders[n] = reader.EntryHeader(n);
    }
}

GM1::Header const& Collection::GetHeader() const
{
    return mHeader;
}

const Surface Collection::GetSurface(size_t index) const
{
    return mEntries.at(index);
}

GM1::EntryHeader const& Collection::GetEntryHeader(size_t index) const
{
    return mHeaders.at(index);
}

GM1::Palette const& Collection::GetPalette(size_t index) const
{
    return mPalettes.at(index);
}

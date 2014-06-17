#include "collection.h"

#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>

#include <boost/filesystem/fstream.hpp>

#include <game/point.h>
#include <game/gm1palette.h>
#include <game/gm1reader.h>
#include <game/tgx.h>

namespace Castle
{
    Collection LoadGM1(const fs::path &path)
    {
        using namespace std::chrono;
        steady_clock::time_point startAt = steady_clock::now();
    
        GM1::GM1Reader gm1;
        try {
            gm1.Open(path, GM1::GM1Reader::Cached);
        } catch(const std::exception &error) {
            std::cerr << "open collection failed: " << error.what() << std::endl;
            throw;
        }

        try {
            Collection temp(gm1);
        
            steady_clock::time_point endAt = steady_clock::now();
            milliseconds elapsed = duration_cast<milliseconds>(endAt - startAt);
            std::clog << "Load " << path << ": " << elapsed.count() << "ms" << std::endl;
        
            return temp;
        } catch(const std::exception &error) {
            std::cerr << "read collection failed: what=" << error.what() << " (path=" << path << ")" << std::endl;
            throw;
        }
    }

    Image LoadTGX(const fs::path &path)
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

    Collection::~Collection() = default;
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

        try {
            for(int n = 0; n < reader.NumEntries(); ++n) {
                mEntries[n] = reader.ReadEntry(n);
                mHeaders[n] = reader.EntryHeader(n);
            }
        } catch(const std::exception &error) {
            std::cerr << "read entry failed: " << error.what() << std::endl;
            throw;
        }
    }

    int Collection::Count() const
    {
        return mHeader.imageCount;
    }

    GM1::Header const& Collection::GetHeader() const
    {
        return mHeader;
    }

    const core::Point Collection::Anchor() const
    {
        return core::Point(mHeader.anchorX, mHeader.anchorY);
    }

    const Image Collection::GetImage(size_t index) const
    {
        return mEntries.at(index);
    }

    GM1::EntryHeader const& Collection::GetEntryHeader(size_t index) const
    {
        return mHeaders.at(index);
    }

    constexpr size_t GetPaletteIndexByName(PaletteName name)
    {
        return static_cast<size_t>(name);
    }

    GM1::Palette const& Collection::GetPalette(PaletteName name) const
    {
        return mPalettes.at(GetPaletteIndexByName(name));
    }
} // namespace Castle

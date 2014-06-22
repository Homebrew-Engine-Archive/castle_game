#include "collection.h"

#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>

#include <boost/filesystem/fstream.hpp>

#include <game/palettename.h>
#include <game/point.h>
#include <game/palette.h>
#include <game/gm1reader.h>
#include <game/tgx.h>

namespace castle
{
    namespace gfx
    {
        Collection LoadGM1(const vfs::path &path)
        {
            using namespace std::chrono;
            steady_clock::time_point startAt = steady_clock::now();
    
            gm1::GM1Reader gm1;
            try {
                gm1.Open(path, gm1::GM1Reader::Cached);
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

        Image LoadTGX(const vfs::path &path)
        {
            boost::filesystem::ifstream fin(path, std::ios_base::binary);

            if(!fin.is_open()) {
                std::cerr << "fail reading " << path << std::endl;
                throw std::runtime_error(strerror(errno));
            }
    
            try {
                return tgx::ReadImage(fin);
            } catch(const std::exception &error) {
                std::cerr << "read image failed: " << error.what() << std::endl;
                throw;
            }
        }

        Collection::~Collection() = default;
        Collection::Collection(const Collection &collection) = default;
        Collection& Collection::operator=(const Collection &collection) = default;

        Collection::Collection(const gm1::GM1Reader &reader)
            : mHeader(reader.Header())
            , mEntries(reader.NumEntries())
            , mHeaders(reader.NumEntries())
        {
            mPalettes.reserve(reader.NumPalettes());
            for(size_t n = 0; n < reader.NumPalettes(); ++n) {
                mPalettes.push_back(reader.Palette(n));
            }

            try {
                for(size_t n = 0; n < reader.NumEntries(); ++n) {
                    mEntries[n] = reader.ReadEntry(n);
                    mHeaders[n] = reader.EntryHeader(n);
                }
            } catch(const std::exception &error) {
                std::cerr << "read entry failed: " << error.what() << std::endl;
                throw;
            }
        }

        size_t Collection::Count() const
        {
            return mHeader.imageCount;
        }

        gm1::Header const& Collection::GetHeader() const
        {
            return mHeader;
        }

        const core::Point Collection::Anchor() const
        {
            return core::Point(mHeader.anchorX, mHeader.anchorY);
        }

        const castle::Image& Collection::GetImage(size_t index) const
        {
            return mEntries.at(index);
        }

        gm1::EntryHeader const& Collection::GetEntryHeader(size_t index) const
        {
            return mHeaders.at(index);
        }

        constexpr size_t GetPaletteIndexByName(const PaletteName &name)
        {
            return static_cast<size_t>(name);
        }

        const Palette& Collection::GetPalette(const PaletteName &name) const
        {
            return mPalettes.at(GetPaletteIndexByName(name));
        }
    } // namespace gfx
} // namespace castle

#include "collection.h"

#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>

#include <boost/filesystem/fstream.hpp>

#include <core/point.h>

#include <game/palettename.h>
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
        {
            mPalettes.reserve(reader.NumPalettes());
            for(size_t n = 0; n < reader.NumPalettes(); ++n) {
                mPalettes.push_back(reader.Palette(n));
            }

            try {
                mEntries.reserve(reader.NumEntries());
                for(size_t n = 0; n < reader.NumEntries(); ++n) {
                    CollectionEntry entry;
                    entry.image = reader.ReadEntry(n);
                    entry.header = reader.EntryHeader(n);
                    mEntries.push_back(std::move(entry));
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

        const gm1::Header& Collection::GetHeader() const
        {
            return mHeader;
        }

        const castle::Image& Collection::GetImage(size_t index) const
        {
            return mEntries.at(index).image;
        }

        const gm1::EntryHeader& Collection::GetEntryHeader(size_t index) const
        {
            return mEntries.at(index).header;
        }

        const castle::Palette& Collection::GetPalette(const PaletteName &name) const
        {
            return mPalettes.at(static_cast<size_t>(name));
        }
    }
}

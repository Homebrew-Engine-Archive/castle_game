#ifndef GM1READER_H_
#define GM1READER_H_

#include "tgxreader.h"
#include "gm1.h"
#include "filesystem.h"
#include "gm1entryreader.h"

namespace GM
{
    class GM1EntryReader;
}

namespace GM
{
    
    class GM1Reader
    {
        FilePath mPath;
        GM::Header mHeader;
        std::vector<GM::ImageHeader> mImageHeaders;
        std::vector<GM::Palette> mPalettes;
        std::vector<uint32_t> mSizes;
        std::vector<uint32_t> mOffsets;
        std::vector<uint8_t> mBuffer;

        /**
         * \brief Calculates size of "static" part of the archive by its header.
         */
        size_t GetPreambleSize(const GM::Header &header) const;
        
    public:
        GM1Reader(FilePath const&);
        GM::ImageHeader const& ImageHeader(size_t index) const;
        GM::Palette const& Palette(size_t index) const;
        GM::Header const& Header() const;
        uint8_t const* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        size_t NumEntries() const;
        size_t NumPalettes() const;
        std::unique_ptr<GM::GM1EntryReader> CreateEntryReader() const;
    };

}

#endif

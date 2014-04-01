#ifndef GM1READER_H_
#define GM1READER_H_

#include "gm1.h"
#include "filesystem.h"

namespace GM1
{
    
    class GM1Reader
    {
        FilePath mPath;
        GM1::Header mHeader;
        std::vector<GM1::EntryHeader> mEntryHeaders;
        std::vector<GM1::Palette> mPalettes;
        std::vector<uint32_t> mSizes;
        std::vector<uint32_t> mOffsets;
        std::vector<char> mBuffer;
        
    public:
        GM1Reader(FilePath const&);
        GM1::EntryHeader const& EntryHeader(size_t index) const;
        GM1::Palette const& Palette(size_t index) const;
        GM1::Header const& Header() const;
        char const* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        int NumEntries() const;
        int NumPalettes() const;
    };

}

#endif

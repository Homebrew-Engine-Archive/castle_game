#ifndef GM1READER_H_
#define GM1READER_H_

#include <game/gm1.h>
#include <game/filesystem.h>
#include <game/surface.h>

namespace GM1
{
    class GM1EntryReader;
}

namespace GM1
{
    
    class GM1Reader
    {
        bool mIsOpened;
        FilePath mPath;
        GM1::Header mHeader;
        std::vector<GM1::EntryHeader> mEntryHeaders;
        std::vector<GM1::Palette> mPalettes;
        std::vector<uint32_t> mSizes;
        std::vector<uint32_t> mOffsets;
        std::vector<char> mBuffer;
        std::unique_ptr<GM1EntryReader> mEntryReader;
        
    public:
        GM1Reader();
        explicit GM1Reader(FilePath);
        void Open(FilePath);
        bool IsOpened() const;
        void Close();
        virtual ~GM1Reader();
        
        GM1::EntryHeader const& EntryHeader(size_t index) const;
        GM1::Palette const& Palette(size_t index) const;
        GM1::Header const& Header() const;
        GM1::Encoding Encoding() const;
        char const* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        size_t EntryOffset(size_t index) const;
        int NumEntries() const;
        int NumPalettes() const;

        GM1::GM1EntryReader const* EntryReader() const;

        Surface Decode(size_t index);
    };

}

#endif

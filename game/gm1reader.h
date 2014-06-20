#ifndef gm1READER_H_
#define gm1READER_H_

#include <cstddef>
#include <cstdint>
#include <memory>

#include <game/gm1.h>
#include <game/filesystem.h>

class ReaderEntryData;

namespace castle
{
    class Image;
    class Palette;
}

namespace gm1
{
    class gm1EntryReader;
}

namespace gm1
{
    class gm1Reader
    {
        bool mIsOpened;
        fs::path mPath;
        std::streamoff mDataOffset;
        gm1::Header mHeader;
        std::vector<castle::Palette> mPalettes;
        std::vector<ReaderEntryData> mEntries;
        std::unique_ptr<gm1EntryReader> mEntryReader;
        
    public:
        enum Flags
        {
            NoFlags = 0,
            Cached = 1,
            CheckSizeCategory = 2
        };

        gm1Reader();
        explicit gm1Reader(fs::path, Flags = NoFlags);
        virtual ~gm1Reader();
        
        void Open(fs::path, Flags);
        bool IsOpened() const;
        void Close();
        const castle::Image ReadEntry(size_t index) const;
        
        gm1::EntryHeader const& EntryHeader(size_t index) const;
        castle::Palette const& Palette(size_t index) const;
        gm1::Header const& Header() const;
        gm1::Encoding Encoding() const;
        char const* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        size_t EntryOffset(size_t index) const;
        int NumEntries() const;
        int NumPalettes() const;

        gm1::gm1EntryReader& EntryReader();
    };
}

#endif

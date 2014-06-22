#ifndef gm1READER_H_
#define gm1READER_H_

#include <cstddef>
#include <cstdint>
#include <memory>

#include <game/gm1.h>

#include <boost/filesystem/path.hpp>

namespace castle
{
    class Image;
    class Palette;
}

namespace gm1
{
    class GM1EntryReader;
}

namespace gm1
{
    class ReaderEntryData;

    class GM1Reader
    {
        bool mIsOpened;
        boost::filesystem::wpath mPath;
        std::streamoff mDataOffset;
        gm1::Header mHeader;
        std::vector<castle::Palette> mPalettes;
        std::vector<ReaderEntryData> mEntries;
        std::unique_ptr<GM1EntryReader> mEntryReader;
        
    public:
        enum Flags
        {
            NoFlags = 0,
            Cached = 1,
            CheckSizeCategory = 2
        };

        explicit GM1Reader(const boost::filesystem::wpath& = boost::filesystem::wpath(), Flags = NoFlags);
        virtual ~GM1Reader();
        
        void Open(const boost::filesystem::wpath&, Flags);
        bool IsOpened() const;
        void Close();
        const castle::Image ReadEntry(size_t index) const;
        
        gm1::EntryHeader const& EntryHeader(size_t index) const;
        castle::Palette const& Palette(size_t index) const;
        gm1::Header const& Header() const;
        gm1::Encoding Encoding() const;
        char const* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        size_t NumEntries() const;
        size_t NumPalettes() const;

        gm1::GM1EntryReader& EntryReader();
    };
}

#endif

#ifndef GM1READER_H_
#define GM1READER_H_

#include <cstddef>
#include <cstdint>
#include <memory>

#include <boost/filesystem/path.hpp>

#include <gm1/gm1.h>

namespace core
{
    class Color;
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
        std::vector<core::Palette> mPalettes;
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

        void SetTransparentColor(const core::Color &color);
        
        const char* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        const core::Image ReadEntry(size_t index) const;
        const gm1::EntryHeader& EntryHeader(size_t index) const;
        const core::Palette& Palette(size_t index) const;
        const gm1::Header& Header() const;
        gm1::ArchiveType ArchiveType() const;
        size_t NumEntries() const;
        size_t NumPalettes() const;
    };
}

#endif  // GM1READER_H_

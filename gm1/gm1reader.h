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

    /// Reader implementation type. Each reader is associated with some
    /// data classes.
    enum class ReaderType
    {
        TGX16,
        ///< related to compressed images, some kinds of animation and fonts
            
        TGX8,
        ///< applicable to palettized animation only
            
        Font,
        ///< special purpose font decoder
            
        TileBox,
        ///< general purpose tile decoder
            
        Box,
        ///< TileBox without Tile
            
        Tile,
        ///< TileBox without Box

        Bitmap
        ///< Every kind of uncompressed images
    };
    
    class GM1Reader
    {        
    public:
        enum Flags
        {
            NoFlags = 0,
            Cached = 1,
            CheckSizeCategory = 2,
            SkipTile = 4,
            SkipBox = 8
        };

        explicit GM1Reader(const boost::filesystem::path& = boost::filesystem::path(), int flags = NoFlags);
        virtual ~GM1Reader();
        
        void Open(const boost::filesystem::path&, int flags);
        bool IsOpened() const;
        void Close();

        void SetTransparentColor(const core::Color &color);
        
        const char* EntryData(size_t index) const;
        size_t EntrySize(size_t index) const;
        const core::Image ReadEntry(size_t index) const;
        const gm1::EntryHeader& EntryHeader(size_t index) const;
        const core::Palette& Palette(size_t index) const;
        const gm1::Header& Header() const;
        ReaderType GetReaderType() const;
        size_t NumEntries() const;
        size_t NumPalettes() const;

        GM1EntryReader& GetEntryReader();
        const GM1EntryReader& GetEntryReader() const;

    private:
        bool mIsOpened;
        int mFlags;
        boost::filesystem::wpath mPath;
        std::streamoff mDataOffset;
        gm1::Header mHeader;
        std::vector<core::Palette> mPalettes;
        std::vector<ReaderEntryData> mEntries;
        std::unique_ptr<GM1EntryReader> mEntryReader;
    };
}

#endif  // GM1READER_H_

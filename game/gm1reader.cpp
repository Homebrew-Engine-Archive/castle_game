#include "gm1reader.h"

#include "tgxreader.h"
#include "filesystem.h"

#include <boost/filesystem/fstream.hpp>
#include "endianness.h"

#include <sstream>
#include <functional>

namespace
{

    GM::Encoding GetEncoding(uint32_t dataClass)
    {
        switch(dataClass) {
        case 1: return GM::Encoding::TGX16;
        case 2: return GM::Encoding::TGX8;
        case 3: return GM::Encoding::TileObject;
        case 4: return GM::Encoding::Font;
        case 5: return GM::Encoding::Bitmap;
        case 6: return GM::Encoding::TGX16;
        case 7: return GM::Encoding::Bitmap;
        default: return GM::Encoding::Unknown;
        }
    }
    
    void Fail(const std::string &file, int line, const std::string &what)
    {
        std::stringstream ss;
        ss << "In " << file << " at " << line << ": " << what;
        throw std::runtime_error(ss.str());
    }
    
    GM::Header ReadHeader(std::istream &in)
    {
        GM::Header hdr;
        hdr.u1             = Endian::ReadLittle<uint32_t>(in);
        hdr.u2             = Endian::ReadLittle<uint32_t>(in);
        hdr.u3             = Endian::ReadLittle<uint32_t>(in);
        hdr.imageCount     = Endian::ReadLittle<uint32_t>(in);
        hdr.u4             = Endian::ReadLittle<uint32_t>(in);
        hdr.dataClass      = Endian::ReadLittle<uint32_t>(in);
        hdr.u5             = Endian::ReadLittle<uint32_t>(in);
        hdr.u6             = Endian::ReadLittle<uint32_t>(in);
        hdr.sizeCategory   = Endian::ReadLittle<uint32_t>(in);
        hdr.u7             = Endian::ReadLittle<uint32_t>(in);
        hdr.u8             = Endian::ReadLittle<uint32_t>(in);
        hdr.u9             = Endian::ReadLittle<uint32_t>(in);
        hdr.width          = Endian::ReadLittle<uint32_t>(in);
        hdr.height         = Endian::ReadLittle<uint32_t>(in);
        hdr.u10            = Endian::ReadLittle<uint32_t>(in);
        hdr.u11            = Endian::ReadLittle<uint32_t>(in);
        hdr.u12            = Endian::ReadLittle<uint32_t>(in);
        hdr.u13            = Endian::ReadLittle<uint32_t>(in);
        hdr.anchorX        = Endian::ReadLittle<uint32_t>(in);
        hdr.anchorY        = Endian::ReadLittle<uint32_t>(in);
        hdr.dataSize       = Endian::ReadLittle<uint32_t>(in);
        hdr.u14            = Endian::ReadLittle<uint32_t>(in);
        return hdr;
    }

    GM::Palette ReadPalette(std::istream &in)
    {
        GM::Palette palette;
        for(uint16_t &entry : palette)
            entry = Endian::ReadLittle<uint16_t>(in);
        return palette;
    }

    GM::ImageHeader ReadImageHeader(std::istream &in)
    {
        GM::ImageHeader hdr;
        hdr.width      = Endian::ReadLittle<uint16_t>(in);
        hdr.height     = Endian::ReadLittle<uint16_t>(in);
        hdr.posX       = Endian::ReadLittle<uint16_t>(in);
        hdr.posY       = Endian::ReadLittle<uint16_t>(in);
        hdr.group      = Endian::ReadLittle<uint8_t>(in);
        hdr.groupSize  = Endian::ReadLittle<uint8_t>(in);
        hdr.tileY      = Endian::ReadLittle<uint16_t>(in);
        hdr.tileOrient = Endian::ReadLittle<uint8_t>(in);
        hdr.hOffset    = Endian::ReadLittle<uint8_t>(in);
        hdr.boxWidth   = Endian::ReadLittle<uint8_t>(in);
        hdr.flags      = Endian::ReadLittle<uint8_t>(in);
        return hdr;
    }

}

namespace GM
{

    GM1Reader::GM1Reader(const FilePath &path)
        : mPath(path)
        , mHeader()
        , mImageHeaders()
        , mPalettes()
        , mBuffer()
    {
        using boost::filesystem::fstream;
        
        fstream fin(path, fstream::binary);
        size_t fsize = fin.rdbuf()->in_avail();

        if(fsize < GM::CollectionHeaderBytes) {
            Fail(__FILE__, __LINE__, "Can't read header");
        }
        
        mHeader = ReadHeader(fin);

        if(fsize < GetPreambleSize(mHeader)) {
            Fail(__FILE__, __LINE__, "Can't read preamble");
        }
        mPalettes.resize(GM::CollectionPaletteCount);
        for(GM::Palette &palette : mPalettes)
            palette = ReadPalette(fin);

        mOffsets.resize(mHeader.imageCount);
        for(uint32_t &offset : mOffsets)
            offset = Endian::ReadLittle<uint32_t>(fin);
        
        mSizes.resize(mHeader.imageCount);
        for(uint32_t &size : mSizes)
            size = Endian::ReadLittle<uint32_t>(fin);
        
        mImageHeaders.resize(mHeader.imageCount);
        for(GM::ImageHeader &hdr : mImageHeaders)
            hdr = ReadImageHeader(fin);

        if(fsize < mHeader.dataSize) {
            Fail(__FILE__, __LINE__, "Can't read entry data");
        }
        
        mBuffer.resize(mHeader.dataSize);
        fin.read(reinterpret_cast<char*>(&mBuffer[0]), mBuffer.size());
    }

    size_t GM1Reader::GetPreambleSize(const GM::Header &header) const
    {
        size_t size = 0;

        /** About 88 bytes on GM::Header **/
        size += GM::CollectionHeaderBytes;

        /** About 10 palettes per file 512 bytes each **/
        size += GM::CollectionPaletteCount * GM::CollectionPaletteBytes;

        /** 32-bit size per entry **/
        size += header.imageCount * sizeof(uint32_t);

        /** 32-bit offset per entry **/
        size += header.imageCount * sizeof(uint32_t);
        
        /** Some GM::ImageHeaders of 16 bytes long **/
        size += header.imageCount * GM::CollectionEntryHeaderBytes;

        return size;
    }
    
    size_t GM1Reader::NumEntries() const
    {
        return mHeader.imageCount;
    }

    size_t GM1Reader::NumPalettes() const
    {
        return GM::CollectionPaletteCount;
    }
    
    uint8_t const* GM1Reader::EntryData(size_t index) const
    {
        return &mBuffer.at(mOffsets.at(index));
    }

    size_t GM1Reader::EntrySize(size_t index) const
    {
        return mSizes.at(index);
    }
    
    GM::Header const& GM1Reader::Header() const
    {
        return mHeader;
    }

    GM::ImageHeader const& GM1Reader::ImageHeader(size_t index) const
    {
        return mImageHeaders.at(index);
    }

    GM::Palette const& GM1Reader::Palette(size_t index) const
    {
        return mPalettes.at(index);
    }

    std::unique_ptr<GM::GM1EntryReader> GM1Reader::CreateEntryReader() const
    {
        switch(GetEncoding(mHeader.dataClass)) {
        case GM::Encoding::TGX8:
        case GM::Encoding::TGX16:
        case GM::Encoding::TileObject:
        case GM::Encoding::Bitmap:
        default:
            return std::unique_ptr<GM::GM1EntryReader>(nullptr);
        }
    }
    
}

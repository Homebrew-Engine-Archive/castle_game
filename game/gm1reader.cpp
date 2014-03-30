#include "gm1reader.h"

#include "filesystem.h"

#include <boost/filesystem/fstream.hpp>
#include "endianness.h"

#include <sstream>
#include <functional>

namespace
{

    GM1::Encoding GetEncoding(uint32_t dataClass)
    {
        switch(dataClass) {
        case 1: return GM1::Encoding::TGX16;
        case 2: return GM1::Encoding::TGX8;
        case 3: return GM1::Encoding::TileObject;
        case 4: return GM1::Encoding::Font;
        case 5: return GM1::Encoding::Bitmap;
        case 6: return GM1::Encoding::TGX16;
        case 7: return GM1::Encoding::Bitmap;
        default: return GM1::Encoding::Unknown;
        }
    }
    
    void Fail(const std::string &file, int line, const std::string &what)
    {
        std::stringstream ss;
        ss << "In " << file << " at " << line << ": " << what;
        throw std::runtime_error(ss.str());
    }
    
    GM1::Header ReadHeader(std::istream &in)
    {
        GM1::Header hdr;
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

    GM1::Palette ReadPalette(std::istream &in)
    {
        GM1::Palette palette;
        for(uint16_t &entry : palette)
            entry = Endian::ReadLittle<uint16_t>(in);
        return palette;
    }

    GM1::EntryHeader ReadEntryHeader(std::istream &in)
    {
        GM1::EntryHeader hdr;
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

namespace GM1
{

    GM1Reader::GM1Reader(const FilePath &path)
        : mPath(path)
        , mHeader()
        , mEntryHeaders()
        , mPalettes()
        , mBuffer()
    {
        using boost::filesystem::fstream;
        
        if(!boost::filesystem::exists(path)) {
            Fail(__FILE__, __LINE__, "No such file");
        }
        
        fstream fin(path, fstream::in | fstream::binary);
        size_t fsize = fin.rdbuf()->in_avail();
        if(fin.fail()) {
            Fail(__FILE__, __LINE__, "Can't read file");
        }


        if(fsize < GM1::CollectionHeaderBytes) {
            Fail(__FILE__, __LINE__, "Can't read header");
        }
        
        mHeader = ReadHeader(fin);

        if(fsize < GetPreambleSize(mHeader)) {
            Fail(__FILE__, __LINE__, "Can't read preamble");
        } 
        mPalettes.resize(GM1::CollectionPaletteCount);
        for(GM1::Palette &palette : mPalettes) {
            palette = ReadPalette(fin);
        }

        mOffsets.resize(mHeader.imageCount);
        for(uint32_t &offset : mOffsets) {
            offset = Endian::ReadLittle<uint32_t>(fin);
        }
        
        mSizes.resize(mHeader.imageCount);
        for(uint32_t &size : mSizes) {
            size = Endian::ReadLittle<uint32_t>(fin);
        }
        
        mEntryHeaders.resize(mHeader.imageCount);
        for(GM1::EntryHeader &hdr : mEntryHeaders) {
            hdr = ReadEntryHeader(fin);
        }

        if(fsize < mHeader.dataSize) {
            Fail(__FILE__, __LINE__, "Can't read entry data");
        }
        
        mBuffer.resize(mHeader.dataSize);
        fin.read(reinterpret_cast<char*>(&mBuffer[0]), mBuffer.size());
    }

    size_t GM1Reader::GetPreambleSize(const GM1::Header &header) const
    {
        size_t size = 0;

        /** About 88 bytes on GM1::Header **/
        size += GM1::CollectionHeaderBytes;

        /** About 10 palettes per file 512 bytes each **/
        size += GM1::CollectionPaletteCount * GM1::CollectionPaletteBytes;

        /** 32-bit size per entry **/
        size += header.imageCount * sizeof(uint32_t);

        /** 32-bit offset per entry **/
        size += header.imageCount * sizeof(uint32_t);
        
        /** Some GM1::EntryHeaders of 16 bytes long **/
        size += header.imageCount * GM1::CollectionEntryHeaderBytes;

        return size;
    }
    
    size_t GM1Reader::NumEntries() const
    {
        return mHeader.imageCount;
    }

    size_t GM1Reader::NumPalettes() const
    {
        return GM1::CollectionPaletteCount;
    }
    
    uint8_t const* GM1Reader::EntryData(size_t index) const
    {
        return &mBuffer.at(mOffsets.at(index));
    }

    size_t GM1Reader::EntrySize(size_t index) const
    {
        return mSizes.at(index);
    }
    
    GM1::Header const& GM1Reader::Header() const
    {
        return mHeader;
    }

    GM1::EntryHeader const& GM1Reader::EntryHeader(size_t index) const
    {
        return mEntryHeaders.at(index);
    }

    GM1::Palette const& GM1Reader::Palette(size_t index) const
    {
        return mPalettes.at(index);
    }
    
}

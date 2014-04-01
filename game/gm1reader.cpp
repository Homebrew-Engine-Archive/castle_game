#include "gm1reader.h"

#include "filesystem.h"

#include <boost/filesystem/fstream.hpp>
#include "endianness.h"

#include <sstream>
#include <functional>

namespace
{
    
    void Fail(const std::string &file, int line, const std::string &what)
    {
        std::stringstream ss;
        ss << "In " << file << " at " << line << ": " << what;
        throw std::runtime_error(ss.str());
    }
    
    std::istream& ReadHeader(std::istream &in, GM1::Header &header)
    {
        header.u1             = Endian::ReadLittle<uint32_t>(in);
        header.u2             = Endian::ReadLittle<uint32_t>(in);
        header.u3             = Endian::ReadLittle<uint32_t>(in);
        header.imageCount     = Endian::ReadLittle<uint32_t>(in);
        header.u4             = Endian::ReadLittle<uint32_t>(in);
        header.dataClass      = Endian::ReadLittle<uint32_t>(in);
        header.u5             = Endian::ReadLittle<uint32_t>(in);
        header.u6             = Endian::ReadLittle<uint32_t>(in);
        header.sizeCategory   = Endian::ReadLittle<uint32_t>(in);
        header.u7             = Endian::ReadLittle<uint32_t>(in);
        header.u8             = Endian::ReadLittle<uint32_t>(in);
        header.u9             = Endian::ReadLittle<uint32_t>(in);
        header.width          = Endian::ReadLittle<uint32_t>(in);
        header.height         = Endian::ReadLittle<uint32_t>(in);
        header.u10            = Endian::ReadLittle<uint32_t>(in);
        header.u11            = Endian::ReadLittle<uint32_t>(in);
        header.u12            = Endian::ReadLittle<uint32_t>(in);
        header.u13            = Endian::ReadLittle<uint32_t>(in);
        header.anchorX        = Endian::ReadLittle<uint32_t>(in);
        header.anchorY        = Endian::ReadLittle<uint32_t>(in);
        header.dataSize       = Endian::ReadLittle<uint32_t>(in);
        header.u14            = Endian::ReadLittle<uint32_t>(in);
        return in;
    }

    std::istream& ReadPalette(std::istream &in, GM1::Palette &palette)
    {
        for(uint16_t &entry : palette)
            entry = Endian::ReadLittle<uint16_t>(in);
        return in;
    }

    std::istream& ReadEntryHeader(std::istream &in, GM1::EntryHeader &header)
    {
        header.width      = Endian::ReadLittle<uint16_t>(in);
        header.height     = Endian::ReadLittle<uint16_t>(in);
        header.posX       = Endian::ReadLittle<uint16_t>(in);
        header.posY       = Endian::ReadLittle<uint16_t>(in);
        header.group      = Endian::ReadLittle<uint8_t>(in);
        header.groupSize  = Endian::ReadLittle<uint8_t>(in);
        header.tileY      = Endian::ReadLittle<uint16_t>(in);
        header.tileOrient = Endian::ReadLittle<uint8_t>(in);
        header.hOffset    = Endian::ReadLittle<uint8_t>(in);
        header.boxWidth   = Endian::ReadLittle<uint8_t>(in);
        header.flags      = Endian::ReadLittle<uint8_t>(in);
        return in;
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
        using boost::filesystem::ifstream;
        
        ifstream fin(path, std::ios_base::binary);
        if(!fin.is_open()) {
            Fail(__FILE__, __LINE__, "Unable to open file");
        }

        fin.seekg(0, std::ios_base::end);
        std::streampos fsize = fin.tellg();
        fin.seekg(0);
        
        if(fsize < GM1::CollectionHeaderBytes) {
            Fail(__FILE__, __LINE__, "File to small to read header");
        }
        
        if(!ReadHeader(fin, mHeader)) {
            Fail(__FILE__, __LINE__, "Unable to read header");
        }

        if(fsize < GetPreambleSize(mHeader)) {
            Fail(__FILE__, __LINE__, "File to small to read preamble");
        } 
        mPalettes.resize(GM1::CollectionPaletteCount);
        for(GM1::Palette &palette : mPalettes) {
            if(!ReadPalette(fin, palette)) {
                Fail(__FILE__, __LINE__, "Unable to read palette");
            }
        }

        mOffsets.resize(mHeader.imageCount);
        for(uint32_t &offset : mOffsets) {
            offset = Endian::ReadLittle<uint32_t>(fin);
            if(!fin) {
                Fail(__FILE__, __LINE__, "Unable to read offset");
            }
        }
        
        mSizes.resize(mHeader.imageCount);
        for(uint32_t &size : mSizes) {
            size = Endian::ReadLittle<uint32_t>(fin);
            if(!fin) {
                Fail(__FILE__, __LINE__, "Unable to read size");
            }
        }
        
        mEntryHeaders.resize(mHeader.imageCount);
        for(GM1::EntryHeader &hdr : mEntryHeaders) {
            if(!ReadEntryHeader(fin, hdr)) {
                Fail(__FILE__, __LINE__, "Unable to read entry header");
            }
        }

        if(fsize < mHeader.dataSize) {
            Fail(__FILE__, __LINE__, "File too small to read data");
        }
        
        mBuffer.resize(mHeader.dataSize);
        fin.read(&mBuffer[0], mBuffer.size());
        if(!fin) {
            Fail(__FILE__, __LINE__, "Unable to read data section");
        }
    }
    
    int GM1Reader::NumEntries() const
    {
        return mHeader.imageCount;
    }

    int GM1Reader::NumPalettes() const
    {
        return GM1::CollectionPaletteCount;
    }
    
    char const* GM1Reader::EntryData(size_t index) const
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

#include "gm1reader.h"

#include <cerrno>
#include <cstring>
#include <sstream>
#include <functional>

#include <boost/filesystem/fstream.hpp>
#include <boost/current_function.hpp>

#include <game/gm1entryreader.h>
#include <game/endianness.h>
#include <game/filesystem.h>

namespace
{
    
    void Fail(const std::string &where, const std::string &what)
    {
        std::stringstream ss;
        ss << "In " << where << " error: " << what;
        throw std::runtime_error(ss.str());
    }
    
    std::istream& ReadHeader(std::istream &in, GM1::Header &header)
    {
        header.u1            = Endian::ReadLittle<uint32_t>(in);
        header.u2            = Endian::ReadLittle<uint32_t>(in);
        header.u3            = Endian::ReadLittle<uint32_t>(in);
        header.imageCount    = Endian::ReadLittle<uint32_t>(in);
        header.u4            = Endian::ReadLittle<uint32_t>(in);
        header.dataClass     = Endian::ReadLittle<uint32_t>(in);
        header.u5            = Endian::ReadLittle<uint32_t>(in);
        header.u6            = Endian::ReadLittle<uint32_t>(in);
        header.sizeCategory  = Endian::ReadLittle<uint32_t>(in);
        header.u7            = Endian::ReadLittle<uint32_t>(in);
        header.u8            = Endian::ReadLittle<uint32_t>(in);
        header.u9            = Endian::ReadLittle<uint32_t>(in);
        header.width         = Endian::ReadLittle<uint32_t>(in);
        header.height        = Endian::ReadLittle<uint32_t>(in);
        header.u10           = Endian::ReadLittle<uint32_t>(in);
        header.u11           = Endian::ReadLittle<uint32_t>(in);
        header.u12           = Endian::ReadLittle<uint32_t>(in);
        header.u13           = Endian::ReadLittle<uint32_t>(in);
        header.anchorX       = Endian::ReadLittle<uint32_t>(in);
        header.anchorY       = Endian::ReadLittle<uint32_t>(in);
        header.dataSize      = Endian::ReadLittle<uint32_t>(in);
        header.u14           = Endian::ReadLittle<uint32_t>(in);
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

    GM1Reader::~GM1Reader() = default;
    
    GM1Reader::GM1Reader()
        : GM1Reader {FilePath(), NoFlags}
    { }
    
    GM1Reader::GM1Reader(FilePath path, Flags flags)
        : mIsOpened(false)
        , mPath()
        , mFlags()
        , mDataOffset(0)
        , mStream()
        , mHeader()
        , mEntryHeaders()
        , mPalettes()
        , mBuffer()
        , mEntries()
        , mEntryReader()
    {
        Open(std::move(path), flags);
    }

    bool GM1Reader::IsOpened() const
    {
        return mIsOpened;
    }

    void GM1Reader::Open(FilePath path, Flags flags)
    {
        mFlags = flags;
        mIsOpened = false;
        mPath = std::move(path);

        mStream.open(mPath, std::ios_base::binary);
        if(!mStream.is_open()) {
            Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
        }

        mStream.seekg(0, std::ios_base::end);
        std::streampos fsize = mStream.tellg();
        mStream.seekg(0);
        
        if(fsize < GM1::CollectionHeaderBytes) {
            Fail(BOOST_CURRENT_FUNCTION, "File too small to read header");
        }
        
        if(!ReadHeader(mStream, mHeader)) {
            Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
        }

        if(fsize < GetPreambleSize(mHeader)) {
            Fail(BOOST_CURRENT_FUNCTION, "File to small to read preamble");
        } 
        mPalettes.resize(GM1::CollectionPaletteCount);
        for(GM1::Palette &palette : mPalettes) {
            if(!ReadPalette(mStream, palette)) {
                Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
            }
        }

        mOffsets.resize(mHeader.imageCount);
        for(uint32_t &offset : mOffsets) {
            offset = Endian::ReadLittle<uint32_t>(mStream);
            if(!mStream) {
                Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
            }
        }
        
        mSizes.resize(mHeader.imageCount);
        for(uint32_t &size : mSizes) {
            size = Endian::ReadLittle<uint32_t>(mStream);
            if(!mStream) {
                Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
            }
        }
        
        mEntryHeaders.resize(mHeader.imageCount);
        for(GM1::EntryHeader &hdr : mEntryHeaders) {
            if(!ReadEntryHeader(mStream, hdr)) {
                Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
            }
        }

        if(fsize < mHeader.dataSize) {
            Fail(BOOST_CURRENT_FUNCTION, "File too small to read data");
        }

        mDataOffset = mStream.tellg();
        if(mFlags & Cached) {
            mBuffer.resize(mHeader.dataSize);
            mStream.read(&mBuffer[0], mBuffer.size());
            if(!mStream) {
                Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
            }
            mStream.close();
        } else {
            mEntries.resize(mHeader.imageCount);
        }

        mEntryReader =
            std::move(
                GM1::CreateEntryReader(Encoding()));
        
        mIsOpened = true;
    }

    void GM1Reader::Close()
    {
        mIsOpened = false;
    }

    GM1::Encoding GM1Reader::Encoding() const
    {
        return GM1::GetEncoding(mHeader.dataClass);
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
        if(mFlags & Cached) {
            return mBuffer.data() + mOffsets.at(index);
        } else {
            std::vector<char> &entryData = mEntries.at(index);
            uint32_t entrySize = mSizes.at(index);
            uint32_t entryOffset = mOffsets.at(index);
            if((entrySize > 0) && (entryData.empty())) {
                mStream.seekg(entryOffset + mDataOffset);
                entryData.resize(entrySize);
                mStream.read(&entryData[0], entrySize);
                if(!mStream) {
                    Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
                }
            }
            // TODO what if it would have length about 0?
            return entryData.data();
        }
    }

    size_t GM1Reader::EntrySize(size_t index) const
    {
        return mSizes.at(index);
    }

    size_t GM1Reader::EntryOffset(size_t index) const
    {
        return mOffsets.at(index);
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

    GM1::GM1EntryReader const& GM1Reader::EntryReader() const
    {
        return *mEntryReader;
    }
    
    Surface GM1Reader::Decode(size_t index)
    {
        return mEntryReader->Load(*this, index);
    }
    
}

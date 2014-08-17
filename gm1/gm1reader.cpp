#include "gm1reader.h"

#include <cerrno>
#include <cstring>

#include <vector>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <core/iohelpers.h>
#include <core/color.h>
#include <core/palette.h>
#include <core/image.h>

#include <gm1/gm1entryreader.h>

namespace
{
    std::istream& ReadHeader(std::istream &in, gm1::Header &header)
    {
        core::ReadLittle(in, header.u1);
        core::ReadLittle(in, header.u2);
        core::ReadLittle(in, header.u3);
        core::ReadLittle(in, header.imageCount);
        core::ReadLittle(in, header.u4);
        core::ReadLittle(in, header.dataClass);
        core::ReadLittle(in, header.u5);
        core::ReadLittle(in, header.u6);
        core::ReadLittle(in, header.sizeCategory);
        core::ReadLittle(in, header.u7);
        core::ReadLittle(in, header.u8);
        core::ReadLittle(in, header.u9);
        core::ReadLittle(in, header.width);
        core::ReadLittle(in, header.height);
        core::ReadLittle(in, header.u10);
        core::ReadLittle(in, header.u11);
        core::ReadLittle(in, header.u12);
        core::ReadLittle(in, header.u13);
        core::ReadLittle(in, header.anchorX);
        core::ReadLittle(in, header.anchorY);
        core::ReadLittle(in, header.dataSize);
        core::ReadLittle(in, header.u14);
        return in;
    }

    std::istream& ReadPalette(std::istream &in, core::Palette &palette)
    {
        for(core::Palette::value_type &entry : palette) {
            uint16_t pixel;
            core::ReadLittle<uint16_t>(in, pixel);
            entry = core::PixelToColor(pixel, gm1::PalettePixelFormat);
        }
        return in;
    }

    std::istream& ReadEntryHeader(std::istream &in, gm1::EntryHeader &header)
    {
        core::ReadLittle(in, header.width);
        core::ReadLittle(in, header.height);
        core::ReadLittle(in, header.posX);
        core::ReadLittle(in, header.posY);
        core::ReadLittle(in, header.group);
        core::ReadLittle(in, header.groupSize);
        core::ReadLittle(in, header.tileY);
        core::ReadLittle(in, header.tileOrient);
        core::ReadLittle(in, header.hOffset);
        core::ReadLittle(in, header.boxWidth);
        core::ReadLittle(in, header.flags);
        return in;
    }
}

namespace gm1
{
    struct ReaderEntryData
    {
        EntryHeader header;
        uint32_t size;
        uint32_t offset;
        mutable std::vector<char> buffer;
    };
    
    GM1Reader::~GM1Reader() = default;
    GM1Reader::GM1Reader(const boost::filesystem::path &path, int flags)
        : mIsOpened(false)
        , mPath(path)
        , mDataOffset(0)
        , mHeader()
        , mPalettes()
        , mEntries()
        , mEntryReader()
    {
        if(boost::filesystem::exists(path)) {
            Open(path, flags);
        }
    }

    bool GM1Reader::IsOpened() const
    {
        return mIsOpened;
    }

    void GM1Reader::Open(const boost::filesystem::path &path, int flags)
    {
        mIsOpened = false;
        mFlags = flags;
        mPalettes.resize(0);
        mEntries.resize(0);
        mDataOffset = 0;
        
        boost::filesystem::ifstream fis(path, std::ios_base::binary);
        if(!fis.is_open()) {
            throw std::runtime_error(strerror(errno));
        }

        fis.seekg(0, std::ios_base::end);
        std::streampos fsize = fis.tellg();
        fis.seekg(0);
        
        if(fsize < gm1::CollectionHeaderBytes) {
            throw std::runtime_error("File too small to read header");
        }
        
        if(!ReadHeader(fis, mHeader)) {
            throw std::runtime_error(strerror(errno));
        }

        if(fsize < GetPreambleSize(mHeader)) {
            throw std::runtime_error("File to small to read preamble");
        } 
        mPalettes.reserve(CollectionPaletteCount);
        for(size_t i = 0; i < CollectionPaletteCount; ++i) {
            core::Palette palette(CollectionPaletteColors);
            if(!ReadPalette(fis, palette)) {
                throw std::runtime_error(strerror(errno));
            }
            mPalettes.push_back(palette);
        }

        mEntries.resize(mHeader.imageCount);
        for(ReaderEntryData &data : mEntries) {
            core::ReadLittle(fis, data.offset);
        }
        if(!fis) {
            throw std::runtime_error(strerror(errno));
        }
        
        for(ReaderEntryData &data : mEntries) {
            core::ReadLittle(fis, data.size);
        }
        if(!fis) {
            throw std::runtime_error(strerror(errno));
        }
        
        for(ReaderEntryData &data : mEntries) {
            if(!ReadEntryHeader(fis, data.header)) {
                throw std::runtime_error(strerror(errno));
            }
        }

        if(fsize < mHeader.dataSize) {
            throw std::runtime_error("File too small to read data");
        }

        mDataOffset = fis.tellg();
        if(flags & Cached) {
            for(ReaderEntryData &entry : mEntries) {
                fis.seekg(mDataOffset + entry.offset, std::ios_base::beg);
                entry.buffer.resize(entry.size);
                fis.read(entry.buffer.data(), entry.size);
            }
            if(!fis) {
                throw std::runtime_error(strerror(errno));
            }
        }

        mEntryReader = std::move(gm1::CreateEntryReader(GetReaderType()));

        mPath = std::move(path);
        mIsOpened = true;
    }

    void GM1Reader::Close()
    {
        mIsOpened = false;
    }
    
    ReaderType GM1Reader::GetReaderType() const
    {
        switch(GetDataClass(mHeader.dataClass)) {
        case DataClass::TGX16:
            return ReaderType::TGX16;
            
        case DataClass::TGX8:
            return ReaderType::TGX8;
                
        case DataClass::TileBox:
            if(mFlags & GM1Reader::TileOnly) {
                return ReaderType::Tile;
            } else {
                return ReaderType::TileBox;
            }
            
        case DataClass::Font:
            return ReaderType::Font;
            
        case DataClass::Bitmap:
            return ReaderType::Bitmap;
            
        case DataClass::TGX16_ConstSize:
            return ReaderType::TGX16;
            
        case DataClass::Bitmap_Other:
            return ReaderType::Bitmap;
            
        default:
            throw std::runtime_error("bad data class");
        }
    }
    
    size_t GM1Reader::NumEntries() const
    {
        return mHeader.imageCount;
    }

    size_t GM1Reader::NumPalettes() const
    {
        return gm1::CollectionPaletteCount;
    }
    
    char const* GM1Reader::EntryData(size_t index) const
    {
        const ReaderEntryData &entry = mEntries.at(index);

        if(!entry.buffer.empty() || (entry.size == 0)) {
            return entry.buffer.data();
        }
        
        try {
            boost::filesystem::ifstream fis(mPath, std::ios_base::binary);
            if(!fis.is_open()) {
                throw std::runtime_error(strerror(errno));
            }
            fis.seekg(entry.offset + mDataOffset);
            entry.buffer.resize(entry.size);
            fis.read(entry.buffer.data(), entry.size);
            if(!fis) {
                throw std::runtime_error(strerror(errno));
            }
        } catch(const std::exception &error) {
            entry.buffer.clear();
            throw;
        }
        
        return entry.buffer.data();
    }

    size_t GM1Reader::EntrySize(size_t index) const
    {
        return mEntries.at(index).size;
    }

    gm1::Header const& GM1Reader::Header() const
    {
        return mHeader;
    }

    gm1::EntryHeader const& GM1Reader::EntryHeader(size_t index) const
    {
        return mEntries.at(index).header;
    }

    core::Palette const& GM1Reader::Palette(size_t index) const
    {
        return mPalettes.at(index);
    }

    void GM1Reader::SetTransparentColor(const core::Color &color)
    {
        if(mEntryReader) {
            mEntryReader->Transparent(color);
        }
    }
    
    const core::Image GM1Reader::ReadEntry(size_t index) const
    {
        const gm1::EntryHeader &header = EntryHeader(index);
        const char *data = EntryData(index);
        const size_t bytesCount = EntrySize(index);
        return mEntryReader->Load(header, data, bytesCount);
    }

    GM1EntryReader& GM1Reader::GetEntryReader()
    {
        return *mEntryReader;
    }

    const GM1EntryReader& GM1Reader::GetEntryReader() const
    {
        return *mEntryReader;
    }
}

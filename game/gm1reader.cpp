#include "gm1reader.h"

#include <cerrno>
#include <cstring>
#include <sstream>
#include <functional>

#include <boost/filesystem/fstream.hpp>

#include <game/gm1palette.h>
#include <game/surface.h>
#include <game/gm1entryreader.h>
#include <game/filesystem.h>
#include <game/iohelpers.h>

struct ReaderEntryData
{
    GM1::EntryHeader header;
    uint32_t size;
    uint32_t offset;
    mutable std::vector<char> buffer;
};

namespace
{
    std::istream& ReadHeader(std::istream &in, GM1::Header &header)
    {
        io::ReadLittle(in, header.u1);
        io::ReadLittle(in, header.u2);
        io::ReadLittle(in, header.u3);
        io::ReadLittle(in, header.imageCount);
        io::ReadLittle(in, header.u4);
        io::ReadLittle(in, header.dataClass);
        io::ReadLittle(in, header.u5);
        io::ReadLittle(in, header.u6);
        io::ReadLittle(in, header.sizeCategory);
        io::ReadLittle(in, header.u7);
        io::ReadLittle(in, header.u8);
        io::ReadLittle(in, header.u9);
        io::ReadLittle(in, header.width);
        io::ReadLittle(in, header.height);
        io::ReadLittle(in, header.u10);
        io::ReadLittle(in, header.u11);
        io::ReadLittle(in, header.u12);
        io::ReadLittle(in, header.u13);
        io::ReadLittle(in, header.anchorX);
        io::ReadLittle(in, header.anchorY);
        io::ReadLittle(in, header.dataSize);
        io::ReadLittle(in, header.u14);
        return in;
    }

    std::istream& ReadPalette(std::istream &in, GM1::Palette &palette)
    {
        for(GM1::Palette::value_type &entry : palette) {
            uint16_t pixel;
            io::ReadLittle<uint16_t>(in, pixel);
            entry = PixelToColor(pixel, GM1::PalettePixelFormat);
        }
        return in;
    }

    std::istream& ReadEntryHeader(std::istream &in, GM1::EntryHeader &header)
    {
        io::ReadLittle(in, header.width);
        io::ReadLittle(in, header.height);
        io::ReadLittle(in, header.posX);
        io::ReadLittle(in, header.posY);
        io::ReadLittle(in, header.group);
        io::ReadLittle(in, header.groupSize);
        io::ReadLittle(in, header.tileY);
        io::ReadLittle(in, header.tileOrient);
        io::ReadLittle(in, header.hOffset);
        io::ReadLittle(in, header.boxWidth);
        io::ReadLittle(in, header.flags);
        return in;
    }
}

namespace GM1
{
    GM1Reader::~GM1Reader() = default;
    
    GM1Reader::GM1Reader()
        : GM1Reader {fs::path(), NoFlags}
    { }
    
    GM1Reader::GM1Reader(fs::path path, Flags flags)
        : mIsOpened(false)
        , mPath(path)
        , mFlags()
        , mDataOffset(0)
        , mHeader()
        , mPalettes()
        , mEntries()
        , mEntryReader()
    {
        if(!mPath.empty()) {
            Open(mPath, flags);
        }
    }

    bool GM1Reader::IsOpened() const
    {
        return mIsOpened;
    }

    void GM1Reader::Open(fs::path path, Flags flags)
    {
        mFlags = flags;
        mIsOpened = false;
        mPath = std::move(path);

        boost::filesystem::ifstream fis(mPath, std::ios_base::binary);
        if(!fis.is_open()) {
            throw std::runtime_error(strerror(errno));
        }

        fis.seekg(0, std::ios_base::end);
        std::streampos fsize = fis.tellg();
        fis.seekg(0);
        
        if(fsize < GM1::CollectionHeaderBytes) {
            throw std::logic_error("File too small to read header");
        }
        
        if(!ReadHeader(fis, mHeader)) {
            throw std::runtime_error(strerror(errno));
        }

        if(fsize < GetPreambleSize(mHeader)) {
            throw std::logic_error("File to small to read preamble");
        } 
        mPalettes.resize(GM1::CollectionPaletteCount);
        for(GM1::Palette &palette : mPalettes) {
            if(!ReadPalette(fis, palette)) {
                throw std::runtime_error(strerror(errno));
            }
        }

        mEntries.resize(mHeader.imageCount);
        for(ReaderEntryData &data : mEntries) {
            io::ReadLittle(fis, data.offset);
        }
        if(!fis) {
            throw std::runtime_error(strerror(errno));
        }
        
        for(ReaderEntryData &data : mEntries) {
            io::ReadLittle(fis, data.size);
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
            throw std::logic_error("File too small to read data");
        }

        mDataOffset = fis.tellg();
        if(mFlags & Cached) {
            for(ReaderEntryData &entry : mEntries) {
                fis.seekg(entry.offset + mDataOffset, std::ios_base::beg);
                entry.buffer.resize(entry.size);
                fis.read(entry.buffer.data(), entry.size);
            }
            if(!fis) {
                throw std::runtime_error(strerror(errno));
            }
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

    size_t GM1Reader::EntryOffset(size_t index) const
    {
        return mEntries.at(index).size;
    }
    
    GM1::Header const& GM1Reader::Header() const
    {
        return mHeader;
    }

    GM1::EntryHeader const& GM1Reader::EntryHeader(size_t index) const
    {
        return mEntries.at(index).header;
    }

    GM1::Palette const& GM1Reader::Palette(size_t index) const
    {
        return mPalettes.at(index);
    }

    GM1::GM1EntryReader& GM1Reader::EntryReader()
    {
        return *mEntryReader;
    }
    
    const Surface GM1Reader::ReadEntry(int index) const
    {
        return mEntryReader->Load(*this, index);
    }
}

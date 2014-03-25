#include "gm1reader.h"

#include "rw.h"
#include "filesystem.h"

#include <functional>

namespace
{

    void MapHeader(std::function<uint32_t()> value, GM::Header &header)
    {
        header.u1           = value();
        header.u2           = value();
        header.u3           = value();
        header.imageCount   = value();
        header.u4           = value();
        header.dataClass    = value();
        header.u5           = value();
        header.u6           = value();
        header.sizeCategory = value();
        header.u7           = value();
        header.u8           = value();
        header.u9           = value();
        header.width        = value();
        header.height       = value();
        header.u10          = value();
        header.u11          = value();
        header.u12          = value();
        header.u13          = value();
        header.anchorX      = value();
        header.anchorY      = value();
        header.dataSize     = value();
        header.u14          = value();
    }
    
}

namespace GM
{

    GMReader::GMReader()
    {
        auto constZero = []() { return 0; };
        MapHeader(constZero, mHeader);
    }
    
    bool GMReader::Open(const FilePath &path)
    {
        
        return true;
    }

    size_t GMReader::EntryCount() const
    {
        return mHeader.imageCount;
    }

    const FileBuffer &GMReader::GetEntryData(size_t index) const
    {
        return mBuffers[index];
    }

    const GM::Header &GMReader::GetHeader() const
    {
        return mHeader;
    }

    const GM::ImageHeader &GMReader::GetImageHeader(size_t index) const
    {
        return mImageHeaders[index];
    }

    const GM::Palette &GMReader::GetPalette(size_t index) const
    {
        return mPalettes[index];
    }
    
}

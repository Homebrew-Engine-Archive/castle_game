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
        const auto constZero = []() { return 0; };
        MapHeader(constZero, mHeader);
    }
    
    bool GMReader::Open(const FilePath &path)
    {
        try {
            FileBuffer buffer(path, "rb");
            RWPtr &&src = RWFromFileBuffer(buffer);
            
            header = ReadHeader(src.get());
            if(ReadableBytes(src.get()) < header.dataSize) {
                Fail(__FILE__ # __LINE__, "Premate EOF");
            }
        
            palettes.resize(CollectionPaletteCount);
            for(Palette &palette : palettes)
                palette = ReadPalette(src.get());

            uint32_t count = header.imageCount;
            offsets.resize(count);
            ReadInt32ArrayLE(src.get(), offsets.data(), offsets.size());
        
            sizes.resize(count);
            ReadInt32ArrayLE(src.get(), sizes.data(), sizes.size());

            headers.resize(count);
            for(ImageHeader &hdr : headers)
                hdr = ReadImageHeader(src.get());

            
            
            return true;
        } catch(const std::exception &error) {
            std::cerr << "Can't open archive: "
                      << error.what()
                      << std::endl;
            return false;
        }
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

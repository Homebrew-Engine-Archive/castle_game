#ifndef GM1READER_H_
#define GM1READER_H_

#include "rw.h"
#include "gm1.h"
#include "filesystem.h"

namespace GM
{
    
    class GMReader
    {
        GM::Header mHeader;
        std::vector<FileBuffer> mBuffers;
        std::vector<GM::ImageHeader> mImageHeaders;
        std::vector<GM::Palette> mPalettes;
    public:
        GMReader();
        GMReader(GMReader const&) = default;
        GMReader(GMReader&&) = default;
        GMReader &operator=(GMReader const&) = default;
        GMReader &operator=(GMReader&&) = default;
        virtual ~GMReader() = default;
        
        bool Open(const FilePath &path);
        
        const GM::ImageHeader &GetImageHeader(size_t index) const;
        
        const GM::Palette &GetPalette(size_t index) const;
        
        const GM::Header &GetHeader() const;
        
        const FileBuffer &GetEntryData(size_t index) const;
        
        size_t EntryCount() const;
    };

}

#endif

#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include <iosfwd>
#include <memory>
#include "tgx.h"
#include "surface.h"

namespace GM1
{
    class GM1Reader;
    class EntryHeader;
    class Header;
}

namespace GM1
{

    class GM1EntryReader
    {
    protected:
        Surface CreateSurface(const GM1::EntryHeader &header) const;
        virtual void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const = 0;
        
    public:
        virtual int Width(GM1::EntryHeader const&) const;
        virtual int Height(GM1::EntryHeader const&) const;
        virtual int Depth() const;
        virtual uint32_t RedMask() const;
        virtual uint32_t GreenMask() const;
        virtual uint32_t BlueMask() const;
        virtual uint32_t AlphaMask() const;
        virtual uint32_t ColorKey() const;
        Surface Load(GM1::GM1Reader &reader, size_t index) const;
        
    };

    std::unique_ptr<GM1EntryReader> CreateEntryReader(GM1::Header const&);
    
}

#endif

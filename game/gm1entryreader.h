#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include <memory>
#include "surface.h"

namespace GM1
{
    class GM1Reader;
}

namespace GM1
{
    
    class GM1EntryReader
    {
    protected:
        virtual int Width(const GM1::EntryHeader &header) const { return header.width; }
        virtual int Height(const GM1::EntryHeader &header) const { return header.height; }
        virtual int Depth() const { return 16; }
        virtual uint32_t RedMask() const { return TGX::RedMask16; }
        virtual uint32_t GreenMask() const { return TGX::GreenMask16; }
        virtual uint32_t BlueMask() const { return TGX::BlueMask16; }
        virtual uint32_t AlphaMask() const  { return TGX::AlphaMask16; }
        virtual uint32_t ColorKey() const { return TGX::Transparent16; }
        
    public:
        Surface ReadSurface(size_t index) const;
    };
    
}

#endif

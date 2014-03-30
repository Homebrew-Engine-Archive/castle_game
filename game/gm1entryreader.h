#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include "surface.h"

namespace GM
{

    class GM1EntryReader
    {
    public:
        Surface CreateCompatibleSurface(int width, int height) const;
        
        virtual int Width(GM::ImageHeader const&) const = 0;
        virtual int Height(GM::ImageHeader const&) const = 0;
        
        virtual void Decode(uint8_t const*, size_t size, Surface&) = 0;
    };
    
}

#endif

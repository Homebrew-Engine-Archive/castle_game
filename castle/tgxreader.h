#ifndef TGXREADER_H_
#define TGXREADER_H_

#include <iosfwd>
#include <SDL.h>
#include "tgx.h"

namespace TGX
{

    class TGXReader
    {
    protected:
        virtual void ReadStream(uint8_t const*, size_t size) = 0;
        virtual void ReadRepeat(uint8_t const*, size_t size) = 0;
        virtual void ReadTransparent(size_t size) = 0;
        virtual void ReadLineFeed() = 0;
    public:
        virtual void Read(uint8_t const*, size_t size) = 0;
    };

    
    
}

#endif

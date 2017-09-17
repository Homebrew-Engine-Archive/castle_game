#ifndef BITMAPWRITER_H_
#define BITMAPWRITER_H_

#include <gmtool/imagewriter.h>

namespace core
{
    class Image;
}

namespace gmtool
{
    struct BitmapWriter : public ImageWriter
    {
        virtual void Write(SDL_RWops *dst, const core::Image &surface) const;
    };
}

#endif // BITMAPWRITER_H_


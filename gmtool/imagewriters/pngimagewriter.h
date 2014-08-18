#ifndef PNGIMAGEWRITER_H_
#define PNGIMAGEWRITER_H_

#include <gmtool/imagewriter.h>

namespace gmtool
{
    struct PNGImageWriter : public ImageWriter
    {
        virtual void Write(SDL_RWops *dst, const core::Image &surface) const;
    };
}

#endif // PNGIMAGEWRITER_H_

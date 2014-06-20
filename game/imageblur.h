#ifndef IMAGEBLUR_H_
#define IMAGEBLUR_H

#include <cstddef>

namespace castle
{
    class Image;
}

namespace castle
{
    namespace gfx
    {
        void BlurImage(Image &surface, size_t radius);
        void BlurImageAlpha(Image &surface, size_t radius);
    }
}

#endif // IMAGEBLUR_H_

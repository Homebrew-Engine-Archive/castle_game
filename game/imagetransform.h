#ifndef IMAGETRANSFORM_H_
#define IMAGETRANSFORM_H_

namespace castle
{
    class Image;
}

namespace core
{
    class Color;
}

namespace castle
{
    void TransformImage(const Image &surface, core::Color(core::Color const&));
}

#endif // IMAGETRANSFORM_H_

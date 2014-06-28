#ifndef IMAGETRANSFORM_H_
#define IMAGETRANSFORM_H_

namespace core
{
    class Image;
    class Color;
}

namespace core
{
    void TransformImage(const Image &surface, core::Color(core::Color const&));
}

#endif // IMAGETRANSFORM_H_

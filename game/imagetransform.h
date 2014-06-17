#ifndef IMAGETRANSFORM_H_
#define IMAGETRANSFORM_H_

namespace Castle
{
    class Image;
}

namespace core
{
    class Color;
}

namespace Castle
{
    void TransformImage(const Image &surface, core::Color(core::Color const&));
}

#endif // IMAGETRANSFORM_H_

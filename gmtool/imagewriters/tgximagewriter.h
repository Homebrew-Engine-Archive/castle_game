#ifndef TGXIMAGEWRITER_H_
#define TGXIMAGEWRITER_H_

#include <gmtool/imagewriter.h>

namespace core
{
    class Image;
}

namespace gmtool
{
    struct TGXImageWriter : public ImageWriter
    {
        virtual void Write(std::ostream &out, const core::Image &surface) const;
    };
}

#endif // TGXIMAGEWRITER_H_

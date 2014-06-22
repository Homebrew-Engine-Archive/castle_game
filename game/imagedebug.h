#ifndef IMAGEDEBUG_H_
#define IMAGEDEBUG_H_

#include <iosfwd>

namespace castle
{
    class Image;
}

namespace castle
{
    class ImageDebug
    {
        const castle::Image &mImg;
    public:
        explicit ImageDebug(const Image &image);
        virtual ~ImageDebug();

        std::ostream& FormatName(std::ostream&) const;
        std::ostream& BlendModeName(std::ostream&) const;
        std::ostream& AlphaMod(std::ostream&) const;
        std::ostream& Dimensions(std::ostream&) const;
        std::ostream& RefCount(std::ostream&) const;
        std::ostream& ColorTable(std::ostream&) const;
    };
}

#endif // IMAGEDEBUG_H_

#ifndef IMAGEWRITER_H_
#define IMAGEWRITER_H_

#include <vector>
#include <iosfwd>
#include <string>
#include <memory>

#include <SDL.h>

namespace core
{
    class Image;
}

namespace gmtool
{
    /// Interface for rendering images
    class ImageWriter
    {
    public:
        typedef std::shared_ptr<ImageWriter> Ptr;
        
        virtual void Write(SDL_RWops *dst, const core::Image &surface) const;
        virtual void Write(std::ostream &out, const core::Image &surface) const;
        ///< default implementation through SDL_RWops
    };
    
    struct RenderFormat
    {
        std::string name;
        ImageWriter::Ptr writer;
    };

    /// Lists all availble writers tagged with their names
    std::vector<RenderFormat> RenderFormats();
}

#endif // IMAGEWRITER_H_

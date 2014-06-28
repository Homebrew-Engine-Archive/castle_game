#ifndef RENDERER_H_
#define RENDERER_H_

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
    struct Renderer
    {
        typedef std::shared_ptr<Renderer> Ptr;
        virtual void RenderToSDL_RWops(SDL_RWops *dst, const core::Image &surface);
        virtual void RenderToStream(std::ostream &out, const core::Image &surface);
    };
    
    struct RenderFormat
    {
        std::string name;
        Renderer::Ptr renderer;
    };

    std::vector<RenderFormat> RenderFormats();
}

#endif // RENDERER_H_

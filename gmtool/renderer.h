#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>
#include <iosfwd>
#include <string>
#include <memory>

#include <SDL.h>

namespace Castle
{
    class Image;
}

namespace GMTool
{
    struct Renderer
    {
        typedef std::shared_ptr<Renderer> Ptr;
        virtual void RenderTo_SDL_RWops(SDL_RWops *dst, const Castle::Image &surface);
        virtual void RenderToStream(std::ostream &out, const Castle::Image &surface);
    };
    
    struct RenderFormat
    {
        std::string name;
        Renderer::Ptr renderer;
    };

    std::vector<RenderFormat> RenderFormats();
}

#endif // RENDERER_H_

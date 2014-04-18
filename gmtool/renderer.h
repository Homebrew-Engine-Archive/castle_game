#ifndef RENDERER_H_
#define RENDERER_H_

#include <iosfwd>
#include <string>
#include <memory>
#include <vector>

#include <game/sdl_utils.h>

#include <SDL.h>

class Surface;

namespace GMTool
{
    struct Renderer
    {
        typedef std::shared_ptr<Renderer> Ptr;
        virtual void RenderTo_SDL_RWops(SDL_RWops *dst, const Surface &surface);
        virtual void RenderToStream(std::ostream &out, const Surface &surface);
    };
    
    struct RenderFormat
    {
        std::string name;
        Renderer::Ptr renderer;
    };

    std::vector<RenderFormat> RenderFormats();
}

#endif // RENDERER_H_

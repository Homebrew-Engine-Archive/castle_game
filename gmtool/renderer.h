#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>
#include <iosfwd>
#include <string>
#include <memory>

#include <SDL.h>

namespace castle
{
    class Image;
}

namespace gmtool
{
    struct renderer
    {
        typedef std::shared_ptr<renderer> Ptr;
        virtual void rendero_SDL_RWops(SDL_RWops *dst, const castle::Image &surface);
        virtual void renderoStream(std::ostream &out, const castle::Image &surface);
    };
    
    struct renderormat
    {
        std::string name;
        renderer::Ptr renderer;
    };

    std::vector<renderormat> renderormats();
}

#endif // RENDERER_H_

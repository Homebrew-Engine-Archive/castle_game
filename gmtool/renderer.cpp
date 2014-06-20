#include "renderer.h"

#include "config_gmtool.h"

#include <vector>
#include <iostream>
#include <stdexcept>

#include <game/sdl_error.h>
#include <game/sdl_utils.h>
#include <game/rw.h>

#include "renderers/bitmap.h"
#include "renderers/tgxrenderer.h"


#if HAVE_SDL2_IMAGE_PNG
#define USE_PNG
#include "renderers/pngrenderer.h"
#endif

namespace gmtool
{
    std::vector<renderormat> renderormats()
    {
        return std::vector<renderormat> {
            {"bmp", renderer::Ptr(new BitmapFormat)}
          , {"tgx", renderer::Ptr(new TGXrenderer)}
            
            #ifdef USE_PNG
          , {"png", renderer::Ptr(new PNGrenderer)}
            #endif
        };
    }

    void renderer::rendero_SDL_RWops(SDL_RWops *dst, const castle::Image &surface)
    {
        throw std::runtime_error("You should implement renderer::rendero_SDL_RWops()");
    }
    
    void renderer::renderoStream(std::ostream &out, const castle::Image &surface)
    {
        RWPtr rw(core::SDL_RWFromOutputStream(out));
        if(rw) {
            rendero_SDL_RWops(rw.get(), surface);
        } else {
            throw sdl_error();
        }
    }
}

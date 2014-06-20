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
    std::vector<RenderFormat> RenderFormats()
    {
        return std::vector<RenderFormat> {
            {"bmp", Renderer::Ptr(new BitmapFormat)}
          , {"tgx", Renderer::Ptr(new TGXRenderer)}
            
            #ifdef USE_PNG
          , {"png", Renderer::Ptr(new PNGRenderer)}
            #endif
        };
    }

    void Renderer::RenderToSDL_RWops(SDL_RWops *dst, const castle::Image &surface)
    {
        throw std::runtime_error("You should implement Renderer::RenderToSDL_RWops()");
    }
    
    void Renderer::RenderToStream(std::ostream &out, const castle::Image &surface)
    {
        RWPtr rw(core::SDL_RWFromOutputStream(out));
        if(rw) {
            RenderToSDL_RWops(rw.get(), surface);
        } else {
            throw sdl_error();
        }
    }
}

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

namespace GMTool
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

    void Renderer::RenderTo_SDL_RWops(SDL_RWops *dst, const Surface &surface)
    {
        throw std::runtime_error("You should implement Renderer::RenderTo_SDL_RWops()");
    }
    
    void Renderer::RenderToStream(std::ostream &out, const Surface &surface)
    {
        RWPtr rw(core::SDL_RWFromOutputStream(out));
        if(rw) {
            RenderTo_SDL_RWops(rw.get(), surface);
        } else {
            throw sdl_error();
        }
    }
}

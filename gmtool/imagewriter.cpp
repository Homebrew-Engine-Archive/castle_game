#include "imagewriter.h"

#include "config_gmtool.h"

#include <iostream>

#include <core/sdl_error.h>
#include <core/sdl_utils.h>
#include <core/rw.h>

#include "imagewriters/bitmapwriter.h"
#include "imagewriters/tgximagewriter.h"

#if HAVE_SDL2_IMAGE_PNG
#define USE_PNG
#include "imagewriters/pngimagewriter.h"
#endif

namespace gmtool
{
    std::vector<RenderFormat> RenderFormats()
    {
        return {
            {"bmp", ImageWriter::Ptr(new BitmapWriter)}
          , {"tgx", ImageWriter::Ptr(new TGXImageWriter)}
            
            #ifdef USE_PNG
          , {"png", ImageWriter::Ptr(new PNGImageWriter)}
            #endif
        };
    }

    void ImageWriter::Write(SDL_RWops *dst, const core::Image &surface) const
    {
    }
    
    void ImageWriter::Write(std::ostream &out, const core::Image &surface) const
    {
        RWPtr rw(core::SDL_RWFromOutputStream(out));
        if(rw) {
            Write(rw.get(), surface);
        } else {
            throw sdl_error();
        }
    }
}

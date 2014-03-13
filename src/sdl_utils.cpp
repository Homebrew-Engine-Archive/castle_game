#include "sdl_utils.h"

void ThrowSDLError(const SDL_Surface *surface)
{
    if(surface == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
}

void ThrowSDLError(int code)
{
    if(code < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDL_Color MakeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return SDL_Color { r, g, b, a };
}

std::ostream &operator<<(std::ostream &out, const SDL_RendererInfo &info)
{           
    out << "\tname: "
              << info.name
              << std::endl;
            
    out << "\tnum_texture_formats: "
              << std::dec
              << info.num_texture_formats
              << std::endl;
            
    out << "\tmax_texture_width: "
              << std::dec
              << info.max_texture_width
              << std::endl;
            
    out << "\tmax_texture_height: "
              << std::dec
              << info.max_texture_height
              << std::endl;

    out << "\ttexture_formats: ";
    if(info.num_texture_formats == 0) {
        out << "None";
    } else {
        for(size_t index = 0; index < info.num_texture_formats; ++index) {
            out << std::hex
                      << info.texture_formats[index]
                      << " ";
        }
    }
    out << std::endl;

    out << "\tflags: ";
    if(info.flags != 0) {
        if(info.flags & SDL_RENDERER_SOFTWARE)
            out << "SDL_RENDERER_SOFTWARE" << " | ";
        if(info.flags & SDL_RENDERER_ACCELERATED)
            out << "SDL_RENDERER_ACCELERATED" << " | ";
        if(info.flags & SDL_RENDERER_PRESENTVSYNC)
            out << "SDL_RENDERER_PRESENTVSYNC" << " | ";
        if(info.flags & SDL_RENDERER_TARGETTEXTURE)
            out << "SDL_RENDERER_TARGETTEXTURE" << " | ";
    } else {
        out << "None";
    }
    out << std::endl;

    return out;
}

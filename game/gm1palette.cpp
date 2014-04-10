#include "gm1palette.h"

#include <SDL.h>

#include <sstream>
#include <array>
#include <vector>

#include <game/tgx.h>
#include <game/sdl_utils.h>

namespace
{

    void Fail(const std::string &where, int line, const std::string &what)
    {
        std::ostringstream oss;
        oss << "In " << where << " at " << line << " fail: " << what;
        throw std::runtime_error(oss.str());
    }
    
}

namespace GM1
{

    PixelFormatPtr GetPaletteFormat()
    {
        PixelFormatPtr ptr(SDL_AllocFormat(TGX::GetPixelFormatEnum()));
        if(!ptr) {
            Fail(__FILE__, __LINE__, SDL_GetError());
        }
        return std::move(ptr);
    }
    
    std::ostream& PrintPalette(std::ostream &out, const Palette &palette)
    {
        int column = 0;
        out << std::hex;
        for(auto color : palette) {
            column++;
            if(column % 16 == 0)
                out << std::endl;
            out << color << ' ';
        }
        return out;
    }
    
    PalettePtr CreateSDLPalette(const GM1::Palette &palette)
    {
        PixelFormatPtr &&format = GetPaletteFormat();
        std::vector<SDL_Color> colors;
        colors.reserve(GM1::CollectionPaletteColors);
        for(PaletteEntry entry : palette) {
            SDL_Color color{0, 0, 0, 0};
            SDL_GetRGBA(entry, format.get(), &color.r, &color.g, &color.b, &color.a);
            colors.push_back(std::move(color));
        }
        return CreateSDLPalette(colors);
    }

    PalettePtr CreateSDLPalette(const std::vector<SDL_Color> &colors)
    {
        PalettePtr ptr(SDL_AllocPalette(colors.size()));
        if(!ptr) {
            Fail(__FILE__, __LINE__, SDL_GetError());
        }
        if(!SDL_SetPaletteColors(ptr.get(), &colors[0], 0, ptr->ncolors) < 0) {
            Fail(__FILE__, __LINE__, SDL_GetError());
        }
        return ptr;
    }
    
}

#include "gm1palette.h"

#include <SDL.h>

#include <iostream>
#include <algorithm>
#include <vector>

#include <game/color.h>
#include <game/modulo.h>
#include <game/tgx.h>
#include <game/sdl_utils.h>

namespace GM1
{
    Palette::Palette()
        : mStorage(
            SDL_AllocPalette(CollectionPaletteColors),
            [](SDL_Palette *palette) {
                SDL_FreePalette(palette);
            })
    {
    }
    
    Palette::value_type const& Palette::operator[](size_type index) const
    {
        return mStorage->colors[index];
    }

    Palette::value_type& Palette::operator[](Palette::size_type index)
    {
        return mStorage->colors[index];
    }
    
    Palette::size_type Palette::Size() const
    {
        return mStorage->ncolors;                        // CollectionPaletteColors
    }
    
    Palette::const_iterator Palette::begin() const
    {
        return mStorage->colors;
    }
    
    Palette::const_iterator Palette::end() const
    {
        return mStorage->colors + Size();
    }
    
    Palette::iterator Palette::begin()
    {
        return mStorage->colors;
    }
    
    Palette::iterator Palette::end()
    {
        return mStorage->colors + Size();
    }

    SDL_Palette& Palette::asSDLPalette()
    {
        return *mStorage;
    }
    
    std::ostream& PrintPalette(std::ostream &out, const Palette &palette)
    {
        int column = 0;
        out << std::hex;
        for(Palette::value_type entry : palette) {
            out << entry << ' ';
            ++column;
            if(core::Mod(column, 16) == 0) {
                out << std::endl;
            }
        }
        return out;
    }
    
    PalettePtr CreateSDLPalette(const GM1::Palette &palette)
    {
        // const PixelFormatPtr format(SDL_AllocFormat(PalettePixelFormat));
        
        // SDL_Color temp;
        // std::vector<SDL_Color> colors;
        // colors.reserve(palette.Size());
        // for(Palette::value_type entry : palette) {
        //     SDL_GetRGBA(entry, format.get(), &temp.r, &temp.g, &temp.b, &temp.a);
        //     colors.push_back(std::move(temp));
        // }
        // return CreateSDLPalette(colors);
        return PalettePtr(nullptr);
    }

    PalettePtr CreateSDLPalette(const std::vector<SDL_Color> &colors)
    {
        // PalettePtr ptr(SDL_AllocPalette(colors.size()));
        // if(!ptr) {
        //     throw sdl_error();
        // }
        // if(!SDL_SetPaletteColors(ptr.get(), colors.data(), 0, ptr->ncolors) < 0) {
        //     throw sdl_error();
        // }
        // return ptr;
        return PalettePtr(nullptr);
    }
    
}

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

    SDL_Palette const& Palette::asSDLPalette() const
    {
        return *mStorage;
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
}

#include "palette.h"

#include <iostream>

#include <SDL.h>

#include <game/sdl_error.h>
#include <game/color.h>
#include <game/modulo.h>

namespace Castle
{
    Palette::Palette(Palette const&) = default;
    Palette& Palette::operator=(Palette const&) = default;
    Palette::Palette(Palette&&) = default;
    Palette& Palette::operator=(Palette&&) = default;
    Palette::~Palette() = default;

    Palette::Palette()
        : mStorage(nullptr)
    {}
    
    constexpr auto paletteDeleter = [](SDL_Palette *palette) {
        SDL_FreePalette(palette);
    };

    Palette::Palette(size_t colors)
    {
        SDL_Palette *tmp = SDL_AllocPalette(colors);
        if(tmp == NULL) {
            throw sdl_error();
        }
        mStorage.reset(tmp, paletteDeleter);
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
        return (Null() ? 0 : mStorage->ncolors);
    }
    
    Palette::const_iterator Palette::begin() const
    {
        return (Null() ? nullptr : mStorage->colors);
    }
    
    Palette::const_iterator Palette::end() const
    {
        return (Null() ? nullptr : (mStorage->colors + Size()));
    }
    
    Palette::iterator Palette::begin()
    {
        return (Null() ? nullptr : (mStorage->colors));
    }
    
    Palette::iterator Palette::end()
    {
        return (Null() ? nullptr : (mStorage->colors + Size()));
    }

    SDL_Palette& Palette::asSDLPalette()
    {
        return *mStorage;
    }
    
    bool Palette::Null() const
    {
        return !static_cast<bool>(mStorage);
    }
    
    bool Palette::operator!() const
    {
        return !Null();
    }
}

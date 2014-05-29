#ifndef GM1PALETTE_H_
#define GM1PALETTE_H_

#include <iosfwd>
#include <memory>

#include <SDL.h>

#include <array>
#include <vector>

#include <game/sdl_utils.h>

class Surface;

namespace GM1
{
    using palette_entry_t = uint16_t;

    constexpr uint32_t PalettePixelFormat = SDL_PIXELFORMAT_RGB555;

    constexpr size_t CollectionPaletteCount = 10;
    constexpr size_t CollectionPaletteColors = 256;
    constexpr size_t CollectionPaletteBytes = CollectionPaletteColors * sizeof(palette_entry_t);
    
    class Palette
    {
        std::shared_ptr<SDL_Palette> mStorage;
        
    public:
        typedef SDL_Color         value_type;
        typedef SDL_Color const*  const_iterator;
        typedef SDL_Color*        iterator;
        typedef size_t            size_type;

        Palette();
        virtual ~Palette() = default;
        
        value_type const& operator[](size_type index) const;
        value_type& operator[](size_type index);
        size_type Size() const;
        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();

        SDL_Palette& asSDLPalette();
    };
    
    enum class PaletteSet : size_t
    {
        Unknown0,
        Blue,
        Red,
        Orange,
        Yellow,
        Purple,
        Black,
        Cyan,
        Green,
        Unknown1
    };

    std::ostream& PrintPalette(std::ostream&, const Palette &palette);
}

#endif

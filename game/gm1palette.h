#ifndef GM1PALETTE_H_
#define GM1PALETTE_H_

#include <iosfwd>

#include <SDL.h>

#include <array>
#include <vector>

#include <game/sdl_utils.h>

namespace GM1
{
    using palette_entry_t = uint16_t;

    constexpr uint32_t PalettePixelFormat = SDL_PIXELFORMAT_RGB555;

    constexpr size_t CollectionPaletteCount = 10;
    constexpr size_t CollectionPaletteColors = 256;
    constexpr size_t CollectionPaletteBytes = CollectionPaletteColors * sizeof(palette_entry_t);
    
    class Palette
    {
        typedef std::array<palette_entry_t, CollectionPaletteColors> storage_type;
        storage_type mArray;
    public:
        typedef storage_type::value_type        value_type;
        typedef storage_type::const_iterator    const_iterator;
        typedef storage_type::iterator          iterator;
        typedef storage_type::size_type         size_type;
        
        constexpr value_type operator[](size_type index) const {
            return mArray[index];
        }

        constexpr size_type Size() const {
            return mArray.size();
        }
        
        inline const_iterator begin() const {
            return mArray.begin();
        }
        
        inline const_iterator end() const {
            return mArray.end();
        }
        
        inline iterator begin() {
            return mArray.begin();
        }

        inline iterator end() {
            return mArray.end();
        }
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
    
    PalettePtr CreateSDLPalette(const Palette &palette);
    PalettePtr CreateSDLPalette(const std::vector<SDL_Color> &colors);
    std::ostream& PrintPalette(std::ostream&, const Palette &palette);
}

#endif

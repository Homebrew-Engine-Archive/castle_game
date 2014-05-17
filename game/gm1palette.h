#ifndef GM1PALETTE_H_
#define GM1PALETTE_H_

#include <SDL.h>

#include <array>
#include <vector>

#include <game/tgx.h>
#include <game/sdl_utils.h>

namespace GM1
{
    typedef uint16_t PaletteEntry;
    
    const size_t CollectionPaletteCount = 10;
    const size_t CollectionPaletteColors = 256;
    const size_t CollectionPaletteBytes = CollectionPaletteColors * sizeof(PaletteEntry);
    
    typedef std::array<PaletteEntry, CollectionPaletteColors> Palette;

    // class Palette : public std::array<PaletteEntry, CollectionPaletteColors>
    // {
    // public:
    //     PixelFormatPtr PixelFormat() const;
    //     PalettePtr toSDLPalette() const;
    // };
    
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

    PixelFormatPtr PaletteFormat();
    
    PalettePtr CreateSDLPalette(Palette const&);

    PalettePtr CreateSDLPalette(std::vector<SDL_Color> const&);
    
    std::ostream& PrintPalette(std::ostream&, Palette const&);
    
}

#endif

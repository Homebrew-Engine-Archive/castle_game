#ifndef TGX_H_
#define TGX_H_

#include <cstdint>
#include <iosfwd>

class Surface;

namespace TGX
{

    /**
     * TGX is a compression approach for images. It is
     * mainly a kind of RLE compression.
     */
    
    const size_t TileBytes = 512;
    const size_t TileWidth = 30;
    const size_t TileHeight = 16;

    // Magenta for 16 bit transparency (not 7c1f as i thought earlier)
    // 1111100000011111
    const uint16_t Transparent16 = 0xF81F;
    
    // Palette's first entry
    const uint8_t Transparent8 = 0;
    
    // Five bits for the RED alert under the uporin
    // Five for the GREEN tea in its pointless being
    // Five for mortal BLUE
    // Doomed to be blue
    // And one bit - for the ALPHA lord on its alpha throne
    // In the land of sprites where some shadows was
    // One bit to rule them all, one bit to find them
    // One bit to bring them all, and in the ARGB bind them
    // In the land of sprites where some shadows was

    /// \note This masks are not swaped neither according to endianness
    const uint32_t AlphaMask16  = 0x00008000;
    const uint32_t RedMask16    = 0x00007c00;
    const uint32_t GreenMask16  = 0x000003e0;
    const uint32_t BlueMask16   = 0x0000001f;
    
    const int AlphaShift16 = 15;
    const int RedShift16 = 11;
    const int GreenShift16 = 5;
    const int BlueShift16 = 0;
    
    /**
     * \brief Extracts RGB color channel by mask and shift.
     * \param mask Sequential bitmask.
     * \param shift Lower color bit index.
     * \return Value from 0 to 255
     */
    constexpr int GetChannel(uint16_t color, int mask, int shift)
    {
        return ((color & mask) >> shift) * 255 / ((0xffff & mask) >> shift);
    }

    constexpr int GetRed(int color)
    {
        return GetChannel(color, RedMask16, RedShift16);
    }

    constexpr int GetGreen(int color)
    {
        return GetChannel(color, GreenMask16, GreenShift16);
    }

    constexpr int GetBlue(int color)
    {
        return GetChannel(color, BlueMask16, BlueShift16);
    }

    constexpr int GetAlpha(int color)
    {
        return GetChannel(color, AlphaMask16, AlphaShift16);
    }

    void DecodeTGX(std::istream&, size_t numBytes, Surface &surface);

    void DecodeBitmap(std::istream&, size_t numBytes, Surface &surface);

    void DecodeTile(std::istream&, size_t numBytes, Surface &surface);

    Surface ReadTGX(std::istream&);

    std::ostream& WriteEncoded(std::ostream&, int width, int height, char const*, size_t numBytes);

    std::ostream& WriteSurface(std::ostream&, const Surface &surface);
    
} // namespace TGX

#endif

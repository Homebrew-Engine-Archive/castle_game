#ifndef TGX_H_
#define TGX_H_

#include <SDL.h>

class Surface;

namespace TGX
{

    const int TileBytes = 512;
    const int TileWidth = 30;
    const int TileHeight = 16;

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
    const uint32_t AlphaMask16 = 0x8000;
    const uint32_t RedMask16 = 0x7c00;   // 0111110000000000
    const uint32_t GreenMask16 = 0x3e0;  // 0000001111100000
    const uint32_t BlueMask16 = 0x1f;    // 0000000000011111

    const uint32_t PixelFormat_TGX  = SDL_PIXELFORMAT_ARGB1555;

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

    constexpr int GetAlpha(int)
    {
        return 255;
    }

    /**
     * \brief Read image from the source file.
     * \param src Source file.
     * \param size Image size in bytes.
     * \param surface Preallocated 8 or 16-bit surface for image to store.
     */
    void DecodeTGX(SDL_RWops *src, int64_t size, Surface &surface);

    /**
     * \brief Read bitmap from the source file.
     * \param src Source file.
     * \param size Bytes to read.
     * \param surface Preallocated 16-bit surface.
     */
    void DecodeUncompressed(SDL_RWops *src, int64_t size, Surface &surface);

    /**
     * \brief Read tile object from the source file.
     * \param src Source file.
     * \param size Bytes to read.
     * \param surface Preallocated 16-bit surface.
     */
    void DecodeTile(SDL_RWops *src, int64_t size, Surface &surface);

    /**
     * \brief Read 16-bit .tgx file from source.
     *
     * Read compressed tgx16 image and TGX::Header.
     *
     * \param src Source file.
     */
    Surface LoadStandaloneImage(SDL_RWops *src);

} // namespace TGX

#endif

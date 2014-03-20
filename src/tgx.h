#ifndef TGX_H_
#define TGX_H_

#include "SDL.h"

class Surface;

const size_t TILE_BYTES = 512;
const size_t TILE_RHOMBUS_WIDTH = 30;
const size_t TILE_RHOMBUS_HEIGHT = 16;
const size_t TILE_RHOMBUS_PIXELS = TILE_RHOMBUS_WIDTH * TILE_RHOMBUS_HEIGHT;

// Magenta for 16 bit transparency (not 7c1f as i thought earlier)
const uint16_t TGX_TRANSPARENT_RGB16 = 0xF81F;

// Palette's first entry
const uint8_t TGX_TRANSPARENT_RGB8 = 0;

// Five bits for the RED alert under the uporin
// Five for the GREEN tea in its pointless being
// Five for mortal BLUE
// Doomed to be blue
// And one bit - for the ALPHA lord on its alpha throne
// In the land of sprites where some shadows was
// One bit to rule them all, one bit to find them
// One bit to bring them all, and in the ARGB bind them
// In the land of sprites where some shadows was
const uint32_t TGX_RGB16_AMASK = 0;
const uint32_t TGX_RGB16_RMASK = 0x7c00;                   // 0111110000000000
const uint32_t TGX_RGB16_GMASK = 0x3e0;                    // 0000001111100000
const uint32_t TGX_RGB16_BMASK = 0x1f;                     // 0000000000011111

const uint32_t SDL_PIXELFORMAT_TGX = SDL_PIXELFORMAT_ARGB1555;

const int TGX_RGB16_ASHIFT = 0;
const int TGX_RGB16_RSHIFT = 11;
const int TGX_RGB16_GSHIFT = 5;
const int TGX_RGB16_BSHIFT = 0;

const int TGX_MAX_BPP = 16;

namespace tgx
{

// Returns color component in range of [0..256)
    constexpr int GetChannel(uint16_t color, int mask, int shift)
    {
        return ((color & mask) >> shift) * 255 / ((0xffff & mask) >> shift);
    }

    constexpr int GetRed(int color)
    {
        return GetChannel(color, TGX_RGB16_RMASK, TGX_RGB16_RSHIFT);
    }

    constexpr int GetGreen(int color)
    {
        return GetChannel(color, TGX_RGB16_GMASK, TGX_RGB16_GSHIFT);
    }

    constexpr int GetBlue(int color)
    {
        return GetChannel(color, TGX_RGB16_BMASK, TGX_RGB16_BSHIFT);
    }

    constexpr int GetAlpha(int)
    {
        return 255;
    }

    int DecodeTGX(SDL_RWops *src, int64_t size, Surface &surface);
    int DecodeUncompressed(SDL_RWops *src, int64_t size, Surface &surface);
    int DecodeTile(SDL_RWops *src, int64_t size, Surface &surface);
    Surface LoadStandaloneImage(SDL_RWops *src);

}

#endif

#ifndef TGX_H_
#define TGX_H_

#include <array>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <SDL2/SDL.h>

#include <cassert>

#include "surface.h"
#include "macrosota.h"
#include "rw.h"

const size_t TILE_BYTES = 512;

// Width of rhombus rows in pixels.
const size_t TILE_PIXELS_PER_ROW[] = {
    2, 6, 10, 14, 18, 22, 26, 30,
    30, 26, 22, 18, 14, 10, 6, 2};

const size_t TILE_RHOMBUS_WIDTH = 30;
const size_t TILE_RHOMBUS_HEIGHT = 16;

const size_t TILE_RHOMBUS_PIXELS = TILE_RHOMBUS_WIDTH * TILE_RHOMBUS_HEIGHT;

// Magenta for 16 bit transparency (not 7c1f as i thought earlier)
const Uint16 TGX_TRANSPARENT_RGB16 = 0xF81F;

// Palette's first entry
const Uint8 TGX_TRANSPARENT_RGB8 = 0;

// Five bits for the RED alert under the uporin
// Five for the GREEN tea in its pointless being
// Five for mortal BLUE
// Doomed to be blue
// And one bit - for the ALPHA lord on its alpha throne
// In the land of grass where some sprites lie
// One bit to rule them all, one bit to find them
// One bit to bring them all, and in the ARGB bind them
// In the land of grass where some sprites lie
const int TGX_RGB16_AMASK = 0;
const int TGX_RGB16_RMASK = 0x7C00;                   // 0111110000000000
const int TGX_RGB16_GMASK = 0x3E0;                    // 0000001111100000
const int TGX_RGB16_BMASK = 0x1F;                     // 0000000000011111

const int TGX_RGB16_ASHIFT = 15;
const int TGX_RGB16_RSHIFT = 10;
const int TGX_RGB16_GSHIFT = 5;
const int TGX_RGB16_BSHIFT = 0;

const int TGX_MAX_BPP = 16;

// union TGXToken
// {
//     Uint8 type;
//     struct
//     {
//         Uint8 length : 5;
//         Uint8 newlineFlag : 1;
//         Uint8 repeatFlag : 1;
//         Uint8 transparentFlag : 1;
//     } data;
// };

enum class TokenType : int {
    Stream = 0,
    Transparent = 1,
    Repeat = 2,
    LineFeed = 4
};

NAMESPACE_BEGIN(tgx)

struct Header
{
    Uint32 width;
    Uint32 height;
};

// Returns color component in range of [0..256)
constexpr int GetChannel(Uint16 color, int mask, int shift)
{
    return ((color & mask) >> shift) * 255 / ((0xffff & mask) >> shift);
}

constexpr int GetRed(Uint16 color)
{
    return GetChannel(color, TGX_RGB16_RMASK, TGX_RGB16_RSHIFT);
}

constexpr int GetGreen(Uint16 color)
{
    return GetChannel(color, TGX_RGB16_GMASK, TGX_RGB16_GSHIFT);
}

constexpr int GetBlue(Uint16 color)
{
    return GetChannel(color, TGX_RGB16_BMASK, TGX_RGB16_BSHIFT);
}

constexpr int GetAlpha(Uint16)
{
    return 255;
}

int DecodeTGX(SDL_RWops *src, Sint64 size, Surface &surface);
int DecodeUncompressed(SDL_RWops *src, Sint64 size, Surface &surface);
int DecodeTile(SDL_RWops *src, Sint64 size, Surface &surface);
Surface LoadStandaloneImage(SDL_RWops *src);

NAMESPACE_END(tgx)

#endif

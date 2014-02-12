#ifndef TGX_H_
#define TGX_H_

#include <iostream>
#include <exception>
#include <algorithm>
#include <SDL2/SDL.h>

#include "errors.h"

// This dimensions used primary for checking
// consistency of data, not for limiting real images.
// However, it is assumed that there is no image larger than this.
const Uint32 MAX_TGX_WIDTH = 4096;
const Uint32 MAX_TGX_HEIGHT = 4096;

const size_t TILE_BYTES = 512;

// Width of rhombus rows in pixels.
const size_t TILE_PIXELS_PER_ROW[] = {
    2, 6, 10, 14, 18, 22, 26, 30,
    30, 26, 22, 18, 14, 10, 6, 2};

const size_t TILE_RHOMBUS_WIDTH = 30;
const size_t TILE_RHOMBUS_HEIGHT = 16;

const size_t TILE_RHOMBUS_PIXELS = TILE_RHOMBUS_WIDTH * TILE_RHOMBUS_HEIGHT;

// Magenta for 16 bit transparency (7c1f as i thought earlier)
const Uint16 TGX_TRANSPARENT_RGB16 = 0xF81F;

// There are no such constant RGB8 value
// Better took it from palette
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
const Uint32 TGX_RGB16_RMASK = 0x7C00;
const Uint32 TGX_RGB16_GMASK = 0x3E0;
const Uint32 TGX_RGB16_BMASK = 0x1F;
const Uint32 TGX_RGB16_AMASK = 0;

const Uint32 TGX_RGB16_ASHIFT = 15;
const Uint32 TGX_RGB16_RSHIFT = 10;
const Uint32 TGX_RGB16_GSHIFT = 5;
const Uint32 TGX_RGB16_BSHIFT = 0;

constexpr int GetChannel(Uint16 color, Uint32 mask, Uint32 shift)
{
    return ((color & mask) >> shift) * 255 / ((0xFFFF & mask) >> shift);
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

constexpr int GetAlpha(Uint16 color)
{
    return GetChannel(color, TGX_RGB16_AMASK, TGX_RGB16_ASHIFT);
}

struct TGXHeader
{
    Uint32 width;
    Uint32 height;
};

typedef Uint8 TGXToken;

// union TGXToken
// {
//     Uint8 tokenByte;
//     struct
//     {
//         Uint8 length : 5;
//         Uint8 newlineFlag : 1;
//         Uint8 repeatFlag : 1;
//         Uint8 transparentFlag : 1;
//     } data;
// };

enum class TokenType : Uint8 {Stream, Transparent, Repeat, Newline};

constexpr TokenType GetTokenType(const TGXToken &token)
{
    return (token & 0x20) ? TokenType::Transparent
        : (token & 0x40) ? TokenType::Repeat
        : (token & 0x80) ? TokenType::Newline
        : TokenType::Stream;
}

constexpr Uint32 GetTokenLength(const TGXToken &token)
{
    // Lo 5 subsequent bits
    return (token & 0x1f) + 1;
}

constexpr const char * GetTokenTypeName(TokenType t)
{
    return t == TokenType::Transparent ? "Transparent"
        : t == TokenType::Stream ? "Stream"
        : t == TokenType::Newline ? "Newline"
        : "Repeat";
}

Sint64 GetAvailableBytes(SDL_RWops *src);

/**
 * Checks whether device has given amount of `bytes' to be read.
 * Returns immediately.
 */
bool CheckBytesAvailable(SDL_RWops *src, Sint64 bytes) throw();

bool CheckTGXSize(Uint32 width, Uint32 height);

void ReadTGXHeader(SDL_RWops *src, TGXHeader *header);
void ReadTGXToken(SDL_RWops *src, TGXToken *token);

void ReadBitmap(SDL_RWops *src, Uint32 size, Uint16 *pixels);

void ReadTile(SDL_RWops *src, Uint16 *pixels);

void ReadTGX16(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, Uint16 *pImg);
void ReadTGX8(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, Uint8 *pImg);

#endif

#ifndef GM1_H_
#define GM1_H_

#include <array>
#include <vector>
#include <algorithm>
#include <memory>
#include <tuple>

#include <SDL2/SDL.h>

#include "tgx.h"
#include "SDLSurface.h"

const size_t GM1_PALETTE_COUNT = 10;
const size_t GM1_PALETTE_COLORS = 256;
const size_t GM1_TGX8_TRANSPARENT_INDEX = 0;
const size_t GM1_TGX8_SHADOW_INDEX = 1;
const size_t GM1_HEADER_FIELDS = 22;
const size_t GM1_IMAGE_HEADER_SIZE = 16;

typedef std::array<Uint16, GM1_PALETTE_COLORS> GM1Palette;

const GM1Palette EMPTY_PALETTE = {{TGX_TRANSPARENT_RGB16}};

// struct GM1Header
// {
//     Uint32 unknown1[3];
//     Uint32 imageCount;
//     Uint32 unknown2;
//     Uint32 imageClass;
//     Uint32 unknown3[2];
//     Uint32 sizeCategory;
//     Uint32 unknown4[3];
//     Uint32 width;
//     Uint32 height;
//     Uint32 unknown5[4];
//     Uint32 anchorX;
//     Uint32 anchorY;
//     Uint32 imageSize;
//     Uint32 unknown6; 
// };

// GM1Header should be represented as constant-size array now an further.
// Array is easier to read (endianness and alignment), easier
// to access (there are only 3 of 21 fields we really need)
// and easier to print. Let's use arrays, guys!
typedef std::array<Uint32, GM1_HEADER_FIELDS> GM1Header;

enum class ImageEncoding : Uint32 {
    TGX16,
    TGX8,
    TileObject,
    Bitmap,
    Unknown
};

enum class TileAlignment : Uint8 {
    Left,
    Right,
    Center,
    None
};

enum class PaletteSet : size_t {
    Blue = 1,
    Red = 2,
    Orange = 3,
    Yellow = 4,
    Purple = 5,
    Black = 6,
    Cyan = 7,
    Green = 8    
};

inline constexpr Uint32 GetGM1ImageCount(const GM1Header &hdr)
{
    return hdr[3];
}

inline constexpr Uint32 GetGM1ImageSize(const GM1Header &hdr)
{
    return hdr[20];
}

inline constexpr Uint32 GetGM1AnchorX(const GM1Header &hdr)
{
    return hdr[18];
}

inline constexpr Uint32 GetGM1AnchorY(const GM1Header &hdr)
{
    return hdr[19];
}

inline constexpr Uint32 GetGM1ImageClass(const GM1Header &hdr)
{
    return hdr[5];
}

struct GM1ImageHeader
{
    Uint16 width;
    Uint16 height;
    Uint16 posX;
    Uint16 posY;
    Uint8 group;
    Uint8 groupSize;
    Uint16 tileY;
    Uint8 tileOrient;
    Uint8 hOffset;
    Uint8 boxWidth;
    Uint8 flags;
};

struct GM1CollectionScheme
{
    explicit GM1CollectionScheme(SDL_RWops *src) throw (EOFError, FormatError);
    GM1Header header;
    std::vector<GM1Palette> palettes;
    std::vector<Uint32> offsets, sizes;
    std::vector<GM1ImageHeader> headers;
};

std::tuple<Uint32, Uint32> EvalSurfaceSize(const GM1Header &gm1, const GM1ImageHeader &header);

std::shared_ptr<SDLSurface> AllocGM1DrawingPlain(const GM1CollectionScheme &scheme);

void LoadEntries(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<std::shared_ptr<SDLSurface>> &entries);

ImageEncoding GetGM1ImageEncoding(const GM1Header &hdr);

std::shared_ptr<SDLSurface>
LoadTGX16Surface(SDL_RWops *src, Uint32 width, Uint32 height, Sint64 size);

std::shared_ptr<SDLSurface>
LoadTGX8Surface(SDL_RWops *src, Uint32 width, Uint32 height, Sint64 size);

std::shared_ptr<SDLSurface>
LoadBitmapSurface(SDL_RWops *src, Uint32 width, Uint32 height, Sint64 size);

std::shared_ptr<SDLSurface>
LoadTileSurface(SDL_RWops *src);

std::shared_ptr<SDLSurface>
LoadTileObjectSurface(SDL_RWops *src, const GM1ImageHeader &header, Sint64 size);

std::shared_ptr<SDLSurface>
LoadDrawingPlain(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<SDL_Rect> &rects);

#endif

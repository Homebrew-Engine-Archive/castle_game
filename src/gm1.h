#ifndef GM1_H_
#define GM1_H_

#include <array>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <exception>

#include <SDL2/SDL.h>

#include "macrosota.h"
#include "errors.h"
#include "tgx.h"
#include "surface.h"
#include "rw.h"

const size_t GM1_PALETTE_COUNT = 10;
const size_t GM1_PALETTE_COLORS = 256;
const size_t GM1_TGX8_TRANSPARENT_INDEX = 0;
const size_t GM1_TGX8_SHADOW_INDEX = 1;
const size_t GM1_IMAGE_HEADER_BYTES = 16;
const size_t GM1_HEADER_FIELDS = 22;
const size_t GM1_HEADER_BYTES = 88;

DEFINE_ERROR_TYPE(GM1Error);

NAMESPACE_BEGIN(gm1)

// struct Header
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
    
// Header should be represented as constant-size array now an further.
// Array is easier to read (endianness and alignment), easier
// to access (there are only 3 of 21 fields we really need)
// and easier to print. Let's use arrays, guys!

typedef std::array<Uint32, GM1_HEADER_FIELDS> Header;

typedef std::array<Uint16, GM1_PALETTE_COLORS> Palette;

enum class Encoding : Uint32 {
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

struct ImageHeader
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

struct Collection
{
    explicit Collection(SDL_RWops *src)
        throw(GM1Error);
    Header header;
    std::vector<Palette> palettes;
    std::vector<Uint32> offsets, sizes;
    std::vector<ImageHeader> headers;
};

inline constexpr Uint32 GetImageCount(const Header &hdr)
{
    return hdr[3];
}

inline constexpr Uint32 GetImageSize(const Header &hdr)
{
    return hdr[20];
}

inline constexpr Uint32 GetAnchorX(const Header &hdr)
{
    return hdr[18];
}

inline constexpr Uint32 GetAnchorY(const Header &hdr)
{
    return hdr[19];
}

inline constexpr Uint32 GetImageClass(const Header &hdr)
{
    return hdr[5];
}

void VerbosePrintImageHeader(const ImageHeader &);
void VerbosePrintHeader(const Header &);
void VerbosePrintPalette(const Palette &);

Surface LoadAtlas(SDL_RWops *src, const Collection &scheme)
    throw(GM1Error, TGXError, SDLError);

std::vector<SDL_Rect> EvalAtlasPartition(const Collection &scheme);

Encoding GetEncoding(const Header &);

const SDL_Palette *CreateSDLPaletteFrom(const Palette &palette);

NAMESPACE_END(gm1)

#endif

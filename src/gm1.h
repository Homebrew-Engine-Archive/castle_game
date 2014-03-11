#ifndef GM1_H_
#define GM1_H_

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <stdexcept>

#include <SDL2/SDL.h>

#include "macrosota.h"
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

NAMESPACE_BEGIN(gm1)

struct Header
{
    Uint32 u1;
    Uint32 u2;
    Uint32 u3;
    Uint32 imageCount;
    Uint32 u4;
    Uint32 dataClass;
    Uint32 u5;
    Uint32 u6;
    Uint32 sizeCategory;
    Uint32 u7;
    Uint32 u8;
    Uint32 u9;
    Uint32 width;
    Uint32 height;
    Uint32 u10;
    Uint32 u11;
    Uint32 u12;
    Uint32 u13;
    Uint32 anchorX;
    Uint32 anchorY;
    Uint32 dataSize;
    Uint32 u14;
};

struct ImageHeader
{
    Uint16 width;
    Uint16 height;
    Uint16 posX;
    Uint16 posY;
    Uint8 group;
    Uint8 groupSize;
    Sint16 tileY;
    Uint8 tileOrient;
    Uint8 hOffset;
    Uint8 boxWidth;
    Uint8 flags;
};

enum class Encoding : Uint32 {
    TGX16,
    TGX8,
    Font,
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

typedef std::array<Uint16, GM1_PALETTE_COLORS> Palette;

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

struct Collection
{
    explicit Collection(SDL_RWops *src)
        throw(std::runtime_error);
    Header header;
    std::vector<Palette> palettes;
    std::vector<Uint32> offsets, sizes;
    std::vector<ImageHeader> headers;

    Encoding encoding() const;
    size_t size() const;
};

void VerbosePrintImageHeader(const ImageHeader &header);
void VerbosePrintHeader(const Header &gm1);
void VerbosePrintPalette(const Palette &palette);
void VerbosePrintCollection(const Collection &collection);

void LoadEntries(SDL_RWops *src, const Collection &scheme, std::vector<Surface> &atlas);

Surface LoadAtlas(SDL_RWops *src, const Collection &scheme)
    throw(std::runtime_error);
PalettePtr CreateSDLPaletteFrom(const Palette &palette);
void PartitionAtlas(const Collection &gm1, std::vector<SDL_Rect> &);

NAMESPACE_END(gm1)

#endif

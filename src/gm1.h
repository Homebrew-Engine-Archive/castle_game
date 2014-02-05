#ifndef GM1_H_
#define GM1_H_

#include <array>
#include <vector>
#include <algorithm>

#include <cassert>

#include <SDL2/SDL.h>

#include "tgx.h"

const size_t GM1_HEADER_FIELDS = 22;

typedef std::array<Uint32, GM1_HEADER_FIELDS> GM1Header;

const size_t GM1_PALETTE_COUNT = 10;

const size_t GM1_PALETTE_SIZE = 256;

typedef std::array<Uint16, GM1_PALETTE_SIZE> GM1Palette;

const GM1Palette EMPTY_PALETTE = {{TGX_TRANSPARENT_RGB16}};

// struct GM1Header
// {
//     Uint32 unknown1[3];
//     Uint32 imageCount;
//     Uint32 unknown2;
//     Uint32 imageClass;
//     Uint32 unknown3[14];
//     Uint32 imageSize;
//     Uint32 unknown4; 
// };

constexpr Uint32 GetGM1ImageCount(const GM1Header &hdr)
{
    return hdr[3];
}

enum class GM1ImageClass : Uint32 {
    TGX16 = 1,                                           // TGX16
    TGX8 = 2,                                            // TGX8
    TileAndTGX16 = 3,                                    // Tile followed by TGX16
    TGX16Font = 4,                                       // TGX16
    Bitmap16 = 5,                                        // Bitmap16
    TGX16ConstSize = 6,                                  // TGX16
    Bitmap16Other = 7                                    // Bitmap16
};

constexpr GM1ImageClass GetGM1ImageClass(const GM1Header &hdr)
{
    return static_cast<GM1ImageClass>(hdr[5]);
}

enum class ImageEncoding : Uint32 {
    TGX16,
    TGX8,
    TileObject,
    Bitmap,
    Unknown
};

constexpr Uint32 GetGM1ImageSize(const GM1Header &hdr)
{
    return hdr[20];
}

const size_t GM1_IMAGE_HEADER_SIZE = 16;

enum class TileAlignment : Uint8 {
    Left,
    Right,
    Center,
    None};

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

void DebugPrint_GM1Header(const GM1Header &header);
void DebugPrint_GM1ImageHeader(const GM1ImageHeader &header);

ImageEncoding GetGM1ImageEncoding(const GM1Header &hdr);
void ReadGM1Header(SDL_RWops *src, GM1Header *hdr);
void ReadGM1Palette(SDL_RWops *src, GM1Palette *pal);
void ReadGM1ImageHeader(SDL_RWops *src, GM1ImageHeader *hdr);

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

struct GM1Image
{
    GM1Image(SDL_RWops *src, Uint32 size, const GM1ImageHeader &hdr, ImageEncoding enc)
        throw (EOFError, FormatError);
 
    GM1ImageHeader header;
    ImageEncoding encoding;
    
    // Written by compressed and uncompressed bitmaps, and tile decorations
    // TGX16, TileObject and Bitmap encodings
    // if encoding is Bitmap => width: header.width, height : header.height - 7
    // if encoding is TGX16 => width: header.width, height : header.height
    // if encoding is TileObject => width: header.boxWidth, height: header.tileY + TILE_RHOMBUS_HEIGHT
    std::vector<Uint16> tgx16buff;

    // Written only by animations (TGX8)
    std::vector<Uint8> tgx8buff;                         // header.width * header.height
    
    // Written only by tile objects (TileObject)
    std::vector<Uint16> tileBuff;                        // TILE_RHOMBUS_HEIGHT * TILE_RHOMBUS_WIDTH

    SDL_Texture * CreateAnimationTexture(SDL_Renderer *renderer, const GM1Palette &pal = EMPTY_PALETTE);
    SDL_Texture * CreateBitmapTexture(SDL_Renderer *renderer, const GM1Palette &pal = EMPTY_PALETTE);

    // Combining tile with image
    SDL_Texture * CreateTileTexture(SDL_Renderer *renderer, const GM1Palette &pal = EMPTY_PALETTE);

    Uint32 Width() const;
    Uint32 Height() const;
};

struct GM1Collection
{
    GM1Collection(SDL_RWops *src) throw (EOFError, FormatError);

    Uint32 ImageCount() const;
    GM1ImageClass ImageClass() const;
    Uint32 ImageSize() const;
    GM1Palette GetPalette(const PaletteSet &ps) const;
    
    GM1Header gm1Header;
    std::vector<GM1Palette> palettes;
    std::vector<GM1Image> images;   
};

struct GM1CollectionScheme
{
    GM1Header header;
    std::vector<GM1Palette> palettes;
    std::vector<Uint32> offsets, sizes;
    std::vector<GM1ImageHeader> headers;
    GM1CollectionScheme(SDL_RWops *src)
        throw (EOFError, FormatError);
};

struct GM1Entry
{
    template<class Image>
    GM1Entry(SDL_RWops *src, Sint64 offset, const GM1CollectionScheme &scheme, size_t index)
        throw (EOFError, FormatError);
};

struct Animation
{
    Animation(const GM1ImageHeader &header);
    void Read(SDL_RWops *src, size_t size);
    Uint32 width;
    Uint32 height;
    std::vector<Uint8> buffer;
};

struct Bitmap
{
    Bitmap(const GM1ImageHeader &header);
    void Read(SDL_RWops *src, size_t size);
    Uint32 width;
    Uint32 height;
    std::vector<Uint16> buffer;
};

struct TileObject
{
    TileObject(const GM1ImageHeader &header);
    void Read(SDL_RWops *src, size_t size);
    Uint32 width;
    Uint32 height;
    Uint32 boxWidth;
    std::vector<Uint16> tile;
    std::vector<Uint16> box;
};

struct TGX16
{
    TGX16(const GM1ImageHeader &header);
    void Read(SDL_RWops *src, size_t size);
    Uint32 width;
    Uint32 height;
    std::vector<Uint16> buffer;
};

#endif

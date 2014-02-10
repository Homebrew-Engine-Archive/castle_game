#ifndef GM1_H_
#define GM1_H_

#include <array>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>

#include "tgx.h"

const size_t GM1_PALETTE_COUNT = 10;

const size_t GM1_PALETTE_SIZE = 256;

const size_t GM1_TGX8_TRANSPARENT_INDEX = 0;
const size_t GM1_TGX8_SHADOW_INDEX = 1;

typedef std::array<Uint16, GM1_PALETTE_SIZE> GM1Palette;

const GM1Palette EMPTY_PALETTE = {{TGX_TRANSPARENT_RGB16}};

const size_t GM1_HEADER_FIELDS = 22;

typedef std::array<Uint32, GM1_HEADER_FIELDS> GM1Header;

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

// const Sint64 GM1_HEADER_SIZE = 88;

constexpr Uint32 GetGM1ImageCount(const GM1Header &hdr)
{
    return hdr[3];
}

const size_t GM1_IMAGE_HEADER_SIZE = 16;

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

constexpr Uint32 GetGM1AnchorX(const GM1Header &hdr)
{
    return hdr[18];
}

constexpr Uint32 GetGM1AnchorY(const GM1Header &hdr)
{
    return hdr[19];
}

ImageEncoding GetGM1ImageEncoding(const GM1Header &hdr);

enum class TileAlignment : Uint8 {
    Left,
    Right,
    Center,
    None
};

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

struct GM1CollectionScheme
{
    explicit GM1CollectionScheme(SDL_RWops *src) throw (EOFError, FormatError);
    GM1Header header;
    std::vector<GM1Palette> palettes;
    std::vector<Uint32> offsets, sizes;
    std::vector<GM1ImageHeader> headers;
};

struct Frame
{
    Frame(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
        throw (EOFError, FormatError);
    SDL_Texture* BuildTexture(SDL_Renderer *renderer, const GM1Palette &palette) const;
    inline Uint8 Get(Uint32 x, Uint32 y) const { return buffer[y * width + x]; }
    const static ImageEncoding encoding = ImageEncoding::TGX8;
    Uint32 width;
    Uint32 height;
    std::vector<Uint8> buffer;
    GM1ImageHeader header;
};

struct Bitmap
{
    Bitmap(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
        throw (EOFError, FormatError);
    SDL_Texture* BuildTexture(SDL_Renderer *renderer) const;
    const static ImageEncoding encoding = ImageEncoding::Bitmap;
    Uint32 width;
    Uint32 height;
    mutable std::vector<Uint16> buffer;
    GM1ImageHeader header;
};

struct TileObject
{
    TileObject(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
        throw (EOFError, FormatError);
    SDL_Texture* BuildTexture(SDL_Renderer *renderer) const;
    const static ImageEncoding encoding = ImageEncoding::TileObject;
    Uint32 width;
    Uint32 height;
    Uint32 boxWidth;
    Uint32 boxOffset;
    mutable std::vector<Uint16> tile;
    mutable std::vector<Uint16> box;
    GM1ImageHeader header;
};

struct TGX16
{
    TGX16(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
        throw (EOFError, FormatError);
    SDL_Texture* BuildTexture(SDL_Renderer *renderer) const;
    const static ImageEncoding encoding = ImageEncoding::TGX16;
    Uint32 width;
    Uint32 height;
    mutable std::vector<Uint16> buffer;
    GM1ImageHeader header;
};

void BlitBuffer(const Uint16 *src, Uint32 srcWidth, Uint32 srcHeight,
                Uint16 *dst, Uint32 dstWidth, Uint32 x, Uint32 y);
void DebugPrint_GM1Header(const GM1Header &header);
void DebugPrint_GM1Palette(const GM1Palette &palette);
void DebugPrint_GM1ImageHeader(const GM1ImageHeader &header);
void ReadGM1Header(SDL_RWops *src, GM1Header *hdr);
void ReadGM1Palette(SDL_RWops *src, GM1Palette *pal);
void ReadGM1ImageHeader(SDL_RWops *src, GM1ImageHeader *hdr);
void ReadFrameSet(SDL_RWops *, const GM1CollectionScheme &, std::vector<Frame> &);
void ReadBitmapSet(SDL_RWops *, const GM1CollectionScheme &, std::vector<Bitmap> &);
void ReadTGX16Set(SDL_RWops *, const GM1CollectionScheme &, std::vector<TGX16> &);
void ReadTileSet(SDL_RWops *, const GM1CollectionScheme &, std::vector<TileObject> &);

#endif

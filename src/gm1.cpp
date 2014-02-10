#include "gm1.h"

// Reinvent SDL_BlitSurface
void BlitBuffer(const Uint16 *src, Uint32 srcWidth, Uint32 srcHeight,
                Uint16 *dst, Uint32 dstWidth, Uint32 x, Uint32 y)
{
    for(Uint32 row = 0; row < srcHeight; ++row) {
        for(Uint32 col = 0; col < srcWidth; ++col) {
            if(src[row*srcWidth+col] != TGX_TRANSPARENT_RGB16)
                dst[(row+y)*dstWidth+col+x] = src[row*srcWidth+col];
        }
    }
}

bool ReadIntArrayLE32(SDL_RWops *src, Uint32 *data, Uint32 count)
{
    if(!CheckBytesAvailable(src, count * sizeof(Uint32)))
        return false;
    for(size_t i = 0; i < count; ++i)
        data[i] = SDL_ReadLE32(src);
    return true;
}

bool ReadIntArrayLE16(SDL_RWops *src, Uint16 *data, Uint32 count)
{
    if(!CheckBytesAvailable(src, count * sizeof(Uint16)))
        return false;
    for(size_t i = 0; i < count; ++i)
        data[i] = SDL_ReadLE16(src);
    return true;
}
ImageEncoding GetGM1ImageEncoding(const GM1Header &hdr)
{
    switch(GetGM1ImageClass(hdr)) {
    case GM1ImageClass::TGX16: return ImageEncoding::TGX16;
    case GM1ImageClass::TGX8: return ImageEncoding::TGX8;
    case GM1ImageClass::TileAndTGX16: return ImageEncoding::TileObject;
    case GM1ImageClass::TGX16Font: return ImageEncoding::TGX16;
    case GM1ImageClass::Bitmap16: return ImageEncoding::Bitmap;
    case GM1ImageClass::TGX16ConstSize: return ImageEncoding::TGX16;
    case GM1ImageClass::Bitmap16Other: return ImageEncoding::Bitmap;
    default: return ImageEncoding::Unknown;
    }
}

const char * GetGM1ImageClassName(GM1ImageClass cl)
{
    switch(cl) {
    case GM1ImageClass::TGX16: return "Compressed 16 bit image";
    case GM1ImageClass::TGX8: return "Compressed animation";
    case GM1ImageClass::TileAndTGX16: return "Tile Object";
    case GM1ImageClass::TGX16Font: return "Compressed font";
    case GM1ImageClass::Bitmap16: return "Uncompressed bitmap";
    case GM1ImageClass::TGX16ConstSize: return "Compressed const size image";
    case GM1ImageClass::Bitmap16Other: return "Uncompressed bitmap (other)";
    default: return "Unknown";
    }
}

void ReadGM1Header(SDL_RWops *src, GM1Header *hdr)
{
    if(!ReadIntArrayLE32(src, &(*hdr)[0], GM1_HEADER_FIELDS))
        throw EOFError("ReadGM1Header");
}

void ReadGM1Palette(SDL_RWops *src, GM1Palette *palette)
{
    if(!ReadIntArrayLE16(src, &(*palette)[0], GM1_PALETTE_SIZE))
        throw EOFError("ReadGM1Palette");
}

void ReadGM1ImageHeader(SDL_RWops *src, GM1ImageHeader *hdr)
{
    if(!CheckBytesAvailable(src, GM1_IMAGE_HEADER_SIZE))
        throw EOFError("ReadGM1ImageHeader");
        
    hdr->width      = SDL_ReadLE16(src);
    hdr->height     = SDL_ReadLE16(src);
    hdr->posX       = SDL_ReadLE16(src);
    hdr->posY       = SDL_ReadLE16(src);
    hdr->group      = SDL_ReadU8(src);
    hdr->groupSize  = SDL_ReadU8(src);
    hdr->tileY      = SDL_ReadLE16(src);
    hdr->tileOrient = SDL_ReadU8(src);
    hdr->hOffset    = SDL_ReadU8(src);
    hdr->boxWidth   = SDL_ReadU8(src);
    hdr->flags      = SDL_ReadU8(src);
}

void DebugPrint_GM1ImageHeader(const GM1ImageHeader &header)
{
    SDL_Log("Width: %d", header.width);
    SDL_Log("Height: %d", header.height);
    SDL_Log("PosX: %d", header.posX);
    SDL_Log("PosY: %d", header.posY);
    SDL_Log("Group: %d", header.group);
    SDL_Log("GroupSize: %d", header.groupSize);
    SDL_Log("TileY: %d", header.tileY);
    SDL_Log("TileOrient: %d", header.tileOrient);
    SDL_Log("Horizontal Offset: %d", header.hOffset);
    SDL_Log("Box Width: %d", header.boxWidth);
    SDL_Log("Flags: %d", header.flags);
}

const char * GetGM1HeaderFieldName(size_t i)
{
    switch(i) {
    case 3: return "Image Count";
    case 5: return "Image Class";
    case 8: return "Size category";
    case 12: return "Width";
    case 13: return "Height";
    case 18: return "Anchor X";
    case 19: return "Anchor Y";
    case 20: return "Image Size";
    default: return "";
    }
}

void DebugPrint_GM1Header(const GM1Header &header)
{
    SDL_Log("ImageCount: %d", GetGM1ImageCount(header));
    SDL_Log("ImageClass: %s", GetGM1ImageClassName(GetGM1ImageClass(header)));
    SDL_Log("ImageSize: %d", GetGM1ImageSize(header));
    for(size_t i = 0; i < GM1_HEADER_FIELDS; ++i) {
        SDL_Log("Field #%d:\t%d --\t%s", i, header[i], GetGM1HeaderFieldName(i));
    }    
}

void DebugPrint_GM1Palette(const GM1Palette &palette)
{
    for(size_t i = 0; i < palette.size(); ++i)
        SDL_Log("# %03d:\t%4x", i, palette[i]);
}

GM1CollectionScheme::GM1CollectionScheme(SDL_RWops *src)
    throw (EOFError, FormatError)
{
    ReadGM1Header(src, &header);
    if(!CheckBytesAvailable(src, GetGM1ImageSize(header)))
        throw EOFError("GetGM1ImageSize");
    auto count = GetGM1ImageCount(header);

    palettes.resize(GM1_PALETTE_COUNT);
    for(auto &palette : palettes)
        ReadGM1Palette(src, &palette);

    offsets.resize(count);
    ReadIntArrayLE32(src, offsets.data(), offsets.size());
    
    sizes.resize(count);
    ReadIntArrayLE32(src, sizes.data(), sizes.size());

    headers.resize(count);
    for(auto &header : headers)
        ReadGM1ImageHeader(src, &header);
}

Frame::Frame(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
    throw (EOFError, FormatError)
    : width(header.width), height(header.height)
    , buffer(width * height, TGX_TRANSPARENT_RGB8)
    , header(header)
{
    ReadTGXImage8(src, size, width, height, buffer.data());
}

SDL_Texture* Frame::BuildTexture(SDL_Renderer *renderer, const GM1Palette &palette) const
{
    std::vector<Uint16> buffer16(buffer.size(), TGX_TRANSPARENT_RGB16);
    for(size_t i = 0; i < buffer.size(); ++i)
        buffer16[i] = palette[buffer[i]];
    auto surface = CreateRGBSurfaceFromTGX16(
        buffer16.data(), width, height);
    SDL_SetColorKey(surface, SDL_TRUE, TGX_TRANSPARENT_RGB16);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

Bitmap::Bitmap(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
    throw (EOFError, FormatError)
    : width(header.width)
      // I don't know why it have been coded this way. It just working.
    , height(header.height - 7)
    , header(header)
{
    ReadBitmap(src, size, buffer.data());
}

SDL_Texture* Bitmap::BuildTexture(SDL_Renderer *renderer) const
{
    auto surface = CreateRGBSurfaceFromTGX16(
        buffer.data(), width, height);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

TGX16::TGX16(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
    throw (EOFError, FormatError)
    : width(header.width)
    , height(header.height)
    , header(header)
{
    ReadTGXImage16(src, size, width, height, buffer.data());
}

SDL_Texture* TGX16::BuildTexture(SDL_Renderer *renderer) const
{
    auto surface = CreateRGBSurfaceFromTGX16(
        buffer.data(), width, height);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

TileObject::TileObject(SDL_RWops *src, const GM1ImageHeader &header, size_t size)
    throw (EOFError, FormatError)
    : width(TILE_RHOMBUS_WIDTH)
    , height(TILE_RHOMBUS_HEIGHT + header.tileY)
    , boxWidth(header.boxWidth)
    , boxOffset(header.hOffset)
    , tile(TILE_RHOMBUS_PIXELS, TGX_TRANSPARENT_RGB16)
    , box(boxWidth * height, TGX_TRANSPARENT_RGB16)
    , header(header)
{
    ReadTile(src, tile.data());
    ReadTGXImage16(src, size - TILE_BYTES, boxWidth, height, box.data());
}

SDL_Texture* TileObject::BuildTexture(SDL_Renderer *renderer) const
{    
    std::vector<Uint16> buffer16(width * height, TGX_TRANSPARENT_RGB16);
    
    BlitBuffer(
        tile.data(), TILE_RHOMBUS_WIDTH, TILE_RHOMBUS_HEIGHT,
        buffer16.data(), width,
        0, height - TILE_RHOMBUS_HEIGHT);
    
    BlitBuffer(
        box.data(), boxWidth, height,
        buffer16.data(), width,
        boxOffset, 0);
    
    auto surface = CreateRGBSurfaceFromTGX16(
        buffer16.data(), width, height);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool CheckCompatible(ImageEncoding lhs, ImageEncoding rhs)
{
    return lhs == rhs;
}

template<class ImageClass>
void ReadSet(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<ImageClass> &images)
{
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);

    if(!CheckCompatible(ImageClass::encoding, GetGM1ImageEncoding(scheme.header)))
        throw FormatError("Invalid image class");

    auto count = GetGM1ImageCount(scheme.header);
    images.reserve(count);
    
    for(size_t index = 0; index < count; ++index) {
        SDL_RWseek(src, origin, RW_SEEK_SET);
        if(SDL_RWseek(src, scheme.offsets[index], RW_SEEK_CUR) < 0)
            throw EOFError("Invalid image offset");
        if(!CheckBytesAvailable(src, scheme.sizes[index])) {
            SDL_Log("There are only %d bytes, but need %d.",
                    (int)GetAvailableBytes(src), (int)scheme.sizes[index]);
            throw EOFError("Invalid image size");
        }
        images.emplace_back(src, scheme.headers[index], scheme.sizes[index]);
    }
}

void ReadFrameSet(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<Frame> &frames)
{
    ReadSet<Frame>(src, scheme, frames);
}

void ReadBitmapSet(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<Bitmap> &bitmaps)
{
    ReadSet<Bitmap>(src, scheme, bitmaps);
}

void ReadTGX16Set(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<TGX16> &images)
{
    ReadSet<TGX16>(src, scheme, images);
}

void ReadTileSet(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<TileObject> &tiles)
{
    ReadSet<TileObject>(src, scheme, tiles);
}


#include "gm1.h"

// Reinvent SDL_BlitSurface
void BlitBuffer(Uint16 *src, Uint32 srcWidth, Uint32 srcHeight,
                Uint16 *dst, Uint32 dstWidth, Uint32 x, Uint32 y)
{
    for(Uint32 row = 0; row < srcHeight; ++row) {
        for(Uint32 col = 0; col < srcWidth; ++col) {
            if(src[row*srcWidth+col] != TGX_TRANSPARENT_RGB16)
                dst[(row+y)*dstWidth+col+x] = src[row*srcWidth+col];
        }
    }
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
    if(!CheckBytesAvailable(src, sizeof(GM1Header)))
        throw EOFError("ReadGM1Header");
    
    for(size_t n = 0; n < GM1_HEADER_FIELDS; ++n)
        (*hdr)[n] = SDL_ReadLE32(src);
}

void ReadGM1Palette(SDL_RWops *src, GM1Palette *palette)
{
    if(!CheckBytesAvailable(src, sizeof(GM1Palette)))
        throw EOFError("ReadGM1Palette");
    
    for(size_t idx = 0; idx < GM1_PALETTE_SIZE; ++idx)
        (*palette)[idx] = SDL_ReadLE16(src);
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

bool ReadInt32Array(SDL_RWops *src, std::vector<Uint32> &arr)
{
    if(!CheckBytesAvailable(src, arr.size() * sizeof(Uint32)))
        return false;
    for(auto &x : arr)
        x = SDL_ReadLE32(src);
    return true;
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

void DebugPrint_GM1Header(const GM1Header &header)
{
    SDL_Log("ImageCount: %d", GetGM1ImageCount(header));
    SDL_Log("ImageClass: %s", GetGM1ImageClassName(GetGM1ImageClass(header)));
    SDL_Log("ImageSize: %d", GetGM1ImageSize(header));
    for(size_t i = 0; i < GM1_HEADER_FIELDS; ++i) {
        SDL_Log("Field #%d: %d", i, header[i]);
    }    
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
    sizes.resize(count);
    for(auto &offset : offsets)
        offset = SDL_ReadLE32(src);
    for(auto &size : sizes)
        size = SDL_ReadLE32(src);

    headers.resize(count);
    for(auto &header : headers)
        ReadGM1ImageHeader(src, &header);
}

template<class Image>
GM1Entry::GM1Entry(SDL_RWops *src, Sint64 offset, const GM1CollectionScheme &scheme, size_t index)
    throw (EOFError, FormatError)
    : Image(scheme.headers[index])
{
    SDL_RWseek(src, offset, RW_SEEK_SET);
    if(SDL_RWseek(src, scheme.offsets[index], RW_SEEK_CUR) < 0)
        throw EOFError("Check image offset");
    if(!CheckBytesAvailable(src, scheme.sizes[index]))
        throw EOFError("Check image size");
    dynamic_cast<Image*>(this)->Read(src, scheme.sizes[index]);
}

Animation::Animation(const GM1ImageHeader &header)
    : width(header.width), height(header.height)
    , buffer(width * height, TGX_TRANSPARENT_RGB8)
{}

void Animation::Read(SDL_RWops *src, size_t size)
{
    ReadTGXImage8(src, size, width, height, buffer.data());
}

Bitmap::Bitmap(const GM1ImageHeader &header)
    : width(header.width)
    , height(header.height)
{
}

void Bitmap::Read(SDL_RWops *src, size_t size)
{
    ReadBitmap(src, size, buffer.data());
}

TGX16::TGX16(const GM1ImageHeader &header)
    : width(header.width)
    , height(header.height)
{
}

void TGX16::Read(SDL_RWops *src, size_t size)
{
    ReadTGXImage16(src, size, width, height, buffer.data());
}

TileObject::TileObject(const GM1ImageHeader &header)
    : width(TILE_RHOMBUS_WIDTH)
    , height(TILE_RHOMBUS_HEIGHT + header.tileY)
    , boxWidth(header.boxWidth)
    , tile(TILE_RHOMBUS_PIXELS, TGX_TRANSPARENT_RGB16)
    , box(boxWidth * height, TGX_TRANSPARENT_RGB16)
{
}

void TileObject::Read(SDL_RWops *src, size_t size)
{
    ReadTile(src, tile.data());
    ReadTGXImage16(src, size, boxWidth, height, box.data());
}

GM1Collection::GM1Collection(SDL_RWops *src) throw (EOFError, FormatError)
{
    ReadGM1Header(src, &gm1Header);
    DebugPrint_GM1Header(gm1Header);

    palettes.resize(GM1_PALETTE_COUNT);
    for(auto &palette : palettes)
        ReadGM1Palette(src, &palette);

    std::vector<Uint32> offsets(ImageCount());
    if(!ReadInt32Array(src, offsets))
        throw EOFError("Reading offsets");
    
    std::vector<Uint32> sizes(ImageCount());
    if(!ReadInt32Array(src, sizes))
        throw EOFError("Reading sizes");
    
    std::vector<GM1ImageHeader> headers(ImageCount());
    for(auto &header : headers)
        ReadGM1ImageHeader(src, &header);
    
    auto origin = SDL_RWseek(src, 0, RW_SEEK_CUR);

    images.reserve(ImageCount());
    for(size_t i = 0; i < ImageCount(); ++i) {
        Uint32 size = sizes[i];
        if(SDL_RWseek(src, origin + offsets[i], RW_SEEK_SET) < 0)
            throw EOFError("Check image offset");
        if(!CheckBytesAvailable(src, size))
            throw EOFError("Check image size");
        images.emplace_back(src, size, headers[i], GetGM1ImageEncoding(gm1Header));
    }
}

Uint32 GM1Collection::ImageCount() const
{
    return GetGM1ImageCount(gm1Header);
}

GM1ImageClass GM1Collection::ImageClass() const
{
    return GetGM1ImageClass(gm1Header);
}

Uint32 GM1Collection::ImageSize() const
{
    return GetGM1ImageSize(gm1Header);
}

GM1Palette GM1Collection::GetPalette(const PaletteSet &ps) const
{
    size_t palIndex = static_cast<size_t>(ps);
    return palettes[palIndex];
}

GM1Image::GM1Image(SDL_RWops *src, Uint32 size, const GM1ImageHeader &hdr, ImageEncoding enc)
    throw (EOFError, FormatError)
    : header(hdr)
    , encoding(enc)
{
    switch(enc) {
    case ImageEncoding::TGX16:
        tgx16buff.resize(Width() * Height(), TGX_TRANSPARENT_RGB16);
        ReadTGXImage16(src, size, Width(), Height(), tgx16buff.data());
        break;
    case ImageEncoding::TGX8:
        tgx8buff.resize(Width() * Height(), TGX_TRANSPARENT_RGB8);
        ReadTGXImage8(src, size, Width(), Height(), tgx8buff.data());
        break;
    case ImageEncoding::TileObject: {
        tileBuff.resize(TILE_RHOMBUS_PIXELS, TGX_TRANSPARENT_RGB16);
        ReadTile(src, tileBuff.data());
        tgx16buff.resize(header.boxWidth * Height(), TGX_TRANSPARENT_RGB16);
        ReadTGXImage16(src, size - TILE_BYTES, header.boxWidth, Height(), tgx16buff.data());
        break;
        }
    case ImageEncoding::Bitmap:
        tgx16buff.resize(size, TGX_TRANSPARENT_RGB16);
        ReadBitmap(src, size, tgx16buff.data());
        break;
    default:
        throw FormatError("Unknown GM1ImageClass");
    }
}

Uint32 GM1Image::Width() const
{
    switch(encoding) {
    case ImageEncoding::TGX16:
        return header.width;
    case ImageEncoding::Bitmap:
        return header.width;
    case ImageEncoding::TGX8:
        return header.width;
    case ImageEncoding::TileObject:
        return TILE_RHOMBUS_WIDTH;
    default:
        return 0;
    }
}

Uint32 GM1Image::Height() const
{
    switch(encoding) {
    case ImageEncoding::TGX16:
        return header.height;
    case ImageEncoding::Bitmap:
        // I don't know why, it is just coded that way
        return header.height - 7;
    case ImageEncoding::TGX8:
        return header.height;
    case ImageEncoding::TileObject:
        return header.tileY + TILE_RHOMBUS_HEIGHT;
    default:
        return 0;
    }
}

SDL_Texture * GM1Image::CreateTileTexture(SDL_Renderer *renderer, const GM1Palette &)
{    
    std::vector<Uint16> resultBuffer(Width() * Height(), TGX_TRANSPARENT_RGB16);
    
    BlitBuffer(tileBuff.data(), TILE_RHOMBUS_WIDTH, TILE_RHOMBUS_HEIGHT,
               resultBuffer.data(), Width(), 0, header.tileY);
    BlitBuffer(tgx16buff.data(), header.boxWidth, Height(),
               resultBuffer.data(), Width(), header.hOffset, 0);
    SDL_Surface *result = CreateRGBSurfaceFromTGX16(
        resultBuffer.data(), Width(), Height());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        renderer, result);
    
    SDL_FreeSurface(result);
    return texture;
}

SDL_Texture * GM1Image::CreateAnimationTexture(SDL_Renderer *renderer, const GM1Palette &palette)
{
    std::vector<Uint16> buff(Width() * Height(), 0);
    for(size_t i = 0; i < buff.size(); ++i) {
        buff[i] = palette[tgx8buff[i]];
    }
    SDL_Surface *surface = CreateRGBSurfaceFromTGX16(
        buff.data(), Width(), Height());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture * GM1Image::CreateBitmapTexture(SDL_Renderer *renderer, const GM1Palette &)
{
    SDL_Surface *surface = CreateRGBSurfaceFromTGX16(
        tgx16buff.data(), Width(), Height());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

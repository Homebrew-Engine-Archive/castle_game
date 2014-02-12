#include "gm1.h"

bool ReadIntSequenceLE32(SDL_RWops *src, Uint32 *data, Uint32 count)
{
    if(!CheckBytesAvailable(src, count * sizeof(Uint32)))
        return false;
    for(size_t i = 0; i < count; ++i)
        data[i] = SDL_ReadLE32(src);
    return true;
}

bool ReadIntSequenceLE16(SDL_RWops *src, Uint16 *data, Uint32 count)
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
    case 1: return ImageEncoding::TGX16;
    case 2: return ImageEncoding::TGX8;
    case 3: return ImageEncoding::TileObject;
    case 4: return ImageEncoding::TGX16;
    case 5: return ImageEncoding::Bitmap;
    case 6: return ImageEncoding::TGX16;
    case 7: return ImageEncoding::Bitmap;
    default: return ImageEncoding::Unknown;
    }
}

const char * GetGM1ImageClassName(Uint32 cl)
{
    switch(cl) {
    case 1: return "Compressed 16 bit image";
    case 2: return "Compressed animation";
    case 3: return "Tile Object";
    case 4: return "Compressed font";
    case 5: return "Uncompressed bitmap";
    case 6: return "Compressed const size image";
    case 7: return "Uncompressed bitmap (other)";
    default: return "Unknown";
    }
}

void ReadGM1Header(SDL_RWops *src, GM1Header *hdr)
{
    if(!ReadIntSequenceLE32(src, &(*hdr)[0], GM1_HEADER_FIELDS))
        throw EOFError("ReadGM1Header");
}

void ReadGM1Palette(SDL_RWops *src, GM1Palette *palette)
{
    if(!ReadIntSequenceLE16(src, &(*palette)[0], GM1_PALETTE_COLORS))
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
        SDL_Log("Field #%d: %d --\t%s", i, header[i], GetGM1HeaderFieldName(i));
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
    Uint32 count = GetGM1ImageCount(header);

    palettes.resize(GM1_PALETTE_COUNT);
    for(GM1Palette &palette : palettes)
        ReadGM1Palette(src, &palette);

    offsets.resize(count);
    ReadIntSequenceLE32(src, offsets.data(), offsets.size());
    
    sizes.resize(count);
    ReadIntSequenceLE32(src, sizes.data(), sizes.size());

    headers.resize(count);
    for(GM1ImageHeader &header : headers)
        ReadGM1ImageHeader(src, &header);
}

GM1Entry::GM1Entry(SDL_RWops *src, const GM1Header &gm1, const GM1ImageHeader &header, Sint64 size)
{
    switch(GetGM1ImageEncoding(gm1)) {
    case ImageEncoding::Bitmap:
        // Bitmap height is 7 pixels less than given. I don't know why.
        surface = std::shared_ptr<SDLSurface>(
            LoadBitmapSurface(src, header.width, header.height - 7, size));
        break;
    case ImageEncoding::TGX16:
        surface = std::shared_ptr<SDLSurface>(
            LoadTGX16Surface(src, header.width, header.height, size));
        break;
    case ImageEncoding::TGX8:
        surface = std::shared_ptr<SDLSurface>(
            LoadTGX8Surface(src, header.width, header.height, size));
        break;
    case ImageEncoding::TileObject:
        surface = std::shared_ptr<SDLSurface>(
            LoadTileObjectSurface(src, header, size));
        break;
    case ImageEncoding::Unknown:
        throw FormatError("Unknown encoding");
    }
}

std::shared_ptr<SDLSurface> LoadTGX16Surface(SDL_RWops *src, Uint32 width, Uint32 height, Sint64 size)
{
    auto frame = std::make_shared<SDLSurface>(
        width, height, 16,
        TGX_RGB16_RMASK,
        TGX_RGB16_GMASK,
        TGX_RGB16_BMASK,
        TGX_RGB16_AMASK);

    frame->SetColorKey(TGX_TRANSPARENT_RGB16);

    Uint16 *bits = reinterpret_cast<Uint16*>(frame->Bits());
    std::fill(bits, bits + width * height, TGX_TRANSPARENT_RGB16);
    ReadTGX16(src, size, width, height, bits);

    return frame;
}

std::shared_ptr<SDLSurface> LoadTGX8Surface(SDL_RWops *src, Uint32 width, Uint32 height, Sint64 size)
{
    auto frame = std::make_shared<SDLSurface>(
        width, height, 8,
        RMASK_DEFAULT,
        GMASK_DEFAULT,
        BMASK_DEFAULT,
        AMASK_DEFAULT);

    frame->SetColorKey(GM1_TGX8_TRANSPARENT_INDEX);

    Uint8 *bits = reinterpret_cast<Uint8*>(frame->Bits());
    std::fill(bits, bits + width * height, GM1_TGX8_TRANSPARENT_INDEX);
    ReadTGX8(src, size, width, height, bits);

    return frame;
}

std::shared_ptr<SDLSurface> LoadBitmapSurface(SDL_RWops *src, Uint32 width, Uint32 height, Sint64 size)
{
    auto bitmap = std::make_shared<SDLSurface>(
        width, height, 16,
        TGX_RGB16_RMASK,
        TGX_RGB16_GMASK,
        TGX_RGB16_BMASK,
        TGX_RGB16_AMASK);

    bitmap->SetColorKey(TGX_TRANSPARENT_RGB16);

    Uint16 *bits = reinterpret_cast<Uint16*>(bitmap->Bits());
    std::fill(bits, bits + width * height, TGX_TRANSPARENT_RGB16);
    ReadBitmap(src, size, bits);

    return bitmap;
}

std::shared_ptr<SDLSurface> LoadTileSurface(SDL_RWops *src)
{
    Uint32 width = TILE_RHOMBUS_WIDTH;
    Uint32 height = TILE_RHOMBUS_HEIGHT;
    
    auto tile = std::make_shared<SDLSurface>(
        width, height, 16,
        TGX_RGB16_RMASK,
        TGX_RGB16_GMASK,
        TGX_RGB16_BMASK,
        TGX_RGB16_AMASK);

    tile->SetColorKey(TGX_TRANSPARENT_RGB16);

    Uint16 *bits = reinterpret_cast<Uint16*>(tile->Bits());
    std::fill(bits, bits + width * height, TGX_TRANSPARENT_RGB16);
    ReadTile(src, bits);

    return tile;
}

std::shared_ptr<SDLSurface> LoadTileObjectSurface(SDL_RWops *src, const GM1ImageHeader &header, Sint64 size)
{
    Uint32 width = TILE_RHOMBUS_WIDTH;
    Uint32 height = TILE_RHOMBUS_HEIGHT + header.tileY;
    
    auto surface = std::make_shared<SDLSurface>(
        width, height, 16,
        TGX_RGB16_RMASK,
        TGX_RGB16_GMASK,
        TGX_RGB16_BMASK,
        TGX_RGB16_AMASK);
    surface->SetColorKey(TGX_TRANSPARENT_RGB16);
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->Bits());
    std::fill(bits, bits + width * height, TGX_TRANSPARENT_RGB16);

    auto tile = LoadTileSurface(src);
    SDLRect tilerect(
        0, height,
        TILE_RHOMBUS_WIDTH, TILE_RHOMBUS_HEIGHT);
    surface->Blit(*tile, tilerect);
    
    auto box = LoadTGX16Surface(
        src, header.boxWidth, height, size - TILE_BYTES);
    SDLRect boxrect(
        header.hOffset, 0,
        header.boxWidth, height);
    surface->Blit(*box, boxrect);

    return surface;
}

void LoadEntries(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<GM1Entry> &entries)
{
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);

    auto count = GetGM1ImageCount(scheme.header);
    entries.reserve(count);

    for(size_t index = 0; index < count; ++index) {
        SDL_RWseek(src, origin, RW_SEEK_SET);
        if(SDL_RWseek(src, scheme.offsets[index], RW_SEEK_CUR) < 0) {
            SDL_Log("RW has only %d bytes, trying offset to %d.",
                    static_cast<int>(SDL_RWsize(src)),
                    static_cast<int>(origin + scheme.offsets[index]));
            throw EOFError("Invalid image offset");
        }
        if(!CheckBytesAvailable(src, scheme.sizes[index])) {
            SDL_Log("There are only %d bytes, but need %d.",
                    static_cast<int>(GetAvailableBytes(src)),
                    static_cast<int>(scheme.sizes[index]));
            throw EOFError("Invalid image size");
        }

        entries.emplace_back(
            src,
            scheme.header,
            scheme.headers[index],
            scheme.sizes[index]);
    }
}

#include "gm1.h"

static void ReadGM1Header(SDL_RWops *src, GM1Header *hdr);
static void ReadGM1Palette(SDL_RWops *src, GM1Palette *palette);
static void ReadGM1ImageHeader(SDL_RWops *src, GM1ImageHeader *hdr);
static bool ReadIntSequenceLE16(SDL_RWops *src, Uint16 *data, Uint32 count);
static bool ReadIntSequenceLE32(SDL_RWops *src, Uint32 *data, Uint32 count);
static const char* GetGM1ImageClassName(Uint32 imageClass);
static const char* GetGM1HeaderFieldName(size_t num);
static void DebugPrint_GM1ImageHeader(const GM1ImageHeader &header);
static void DebugPrint_GM1Header(const GM1Header &header);
static void DebugPrint_GM1Palette(const GM1Palette &palette);

std::tuple<Uint32, Uint32> EvalSurfaceSize(const GM1Header &gm1, const GM1ImageHeader &header)
{
    switch(GetGM1ImageEncoding(gm1)) {
    case ImageEncoding::TGX16:
        return std::make_tuple(header.width, header.height);
    case ImageEncoding::TGX8:
        return std::make_tuple(header.width, header.height);
    case ImageEncoding::TileObject:
        return std::make_tuple(TILE_RHOMBUS_WIDTH, TILE_RHOMBUS_HEIGHT + header.tileY);
    case ImageEncoding::Bitmap:
        return std::make_tuple(header.width, header.height - 7);
    default:
        return std::make_tuple(0, 0);
    }
}

std::shared_ptr<SDLSurface> AllocGM1DrawingPlain(const GM1CollectionScheme &scheme)
{
    Uint32 width = 0;
    Uint32 height = 0;
    
    for(const auto &header : scheme.headers) {
        Uint32 w, h;
        std::tie(w, h) = EvalSurfaceSize(scheme.header, header);
        width = std::max(width, w + header.posX);
        height = std::max(height, h + header.posY);
    }

    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    Uint32 colorKey;
    Uint32 depth;
    
    if(GetGM1ImageEncoding(scheme.header) == ImageEncoding::TGX8) {
        rmask = RMASK_DEFAULT;
        gmask = GMASK_DEFAULT;
        bmask = BMASK_DEFAULT;
        amask = AMASK_DEFAULT;
        colorKey = GM1_TGX8_TRANSPARENT_INDEX;
        depth = 8;
    } else {
        rmask = TGX_RGB16_RMASK;
        gmask = TGX_RGB16_GMASK;
        bmask = TGX_RGB16_BMASK;
        amask = TGX_RGB16_AMASK;
        colorKey = TGX_TRANSPARENT_RGB16;
        depth = 16;
    }

    std::shared_ptr<SDLSurface> surface =
        std::make_shared<SDLSurface>(
            width, height, depth,
            rmask, gmask, bmask, amask);

    surface->SetColorKey(colorKey);
    
    return surface;
}

struct TGX8
{
    static Uint32 Width(const GM1ImageHeader &header) {
        return header.width;
    }
    static Uint32 Height(const GM1ImageHeader &header) {
        return header.height;
    }
    static Uint32 Depth() {
        return 8;
    }
    static Uint32 RedMask() {
        return RMASK_DEFAULT;
    }
    static Uint32 GreenMask() {
        return GMASK_DEFAULT;
    }
    static Uint32 BlueMask() {
        return BMASK_DEFAULT;
    }
    static Uint32 AlphaMask() {
        return AMASK_DEFAULT;
    }
    static void Load(SDL_RWops *src, Sint64 size, const GM1ImageHeader &header, std::shared_ptr<SDLSurface> surface) {
        auto buffer = LoadTGX8Surface(src, Width(header), Height(header), size);
        auto rect = MakeRect(header.posX, header.posY, Width(header), Height(header));
        surface->Blit(*buffer, &rect);
    }
};

struct TileObject
{
    static Uint32 Width(const GM1ImageHeader &) {
        return TILE_RHOMBUS_WIDTH;
    }
    static Uint32 Height(const GM1ImageHeader &header) {
        return TILE_RHOMBUS_HEIGHT + header.tileY;
    }
    static Uint32 Depth() {
        return 16;
    }
    static Uint32 RedMask() {
        return TGX_RGB16_RMASK;
    }
    static Uint32 GreenMask() {
        return TGX_RGB16_GMASK;
    }
    static Uint32 BlueMask() {
        return TGX_RGB16_BMASK;
    }
    static Uint32 AlphaMask() {
        return TGX_RGB16_AMASK;
    }
    static void Load(SDL_RWops *src, Sint64 size, const GM1ImageHeader &header, std::shared_ptr<SDLSurface> surface) {
        auto tile = LoadTileSurface(src);
        auto tilerect = MakeRect(header.posX, header.posY + header.tileY, Width(header), TILE_RHOMBUS_HEIGHT);
        surface->Blit(*tile, &tilerect);
        auto box = LoadTGX16Surface(src, Width(header), Height(header), size - TILE_BYTES);
        auto boxrect = MakeRect(header.posX + header.hOffset, header.posY, header.boxWidth, Height(header));
        surface->Blit(*box, &boxrect);
    }
};

struct TGX16
{
    static Uint32 Width(const GM1ImageHeader &header) {
        return header.width;
    }
    static Uint32 Height(const GM1ImageHeader &header) {
        return header.height;
    }
    static Uint32 Depth() {
        return 16;
    }
    static Uint32 RedMask() {
        return TGX_RGB16_RMASK;
    }
    static Uint32 GreenMask() {
        return TGX_RGB16_GMASK;
    }
    static Uint32 BlueMask() {
        return TGX_RGB16_BMASK;
    }
    static Uint32 AlphaMask() {
        return TGX_RGB16_AMASK;
    }
    static void Load(SDL_RWops *src, Sint64 size, const GM1ImageHeader &header, std::shared_ptr<SDLSurface> surface) {
        auto buffer = LoadTGX16Surface(src, Width(header), Height(header), size);
        auto rect = MakeRect(header.posX, header.posY, Width(header), Height(header));
        surface->Blit(*buffer, &rect);
    }
};

struct Bitmap
{
    static Uint32 Width(const GM1ImageHeader &header) {
        return header.width;
    }
    static Uint32 Height(const GM1ImageHeader &header) {
        return header.height - 7;
    }
    static Uint32 Depth() {
        return 16;
    }
    static Uint32 RedMask() {
        return TGX_RGB16_RMASK;
    }
    static Uint32 GreenMask() {
        return TGX_RGB16_GMASK;
    }
    static Uint32 BlueMask() {
        return TGX_RGB16_BMASK;
    }
    static Uint32 AlphaMask() {
        return TGX_RGB16_AMASK;
    }
    static void Load(SDL_RWops *src, Sint64 size, const GM1ImageHeader &header, std::shared_ptr<SDLSurface> surface) {
        auto buffer = LoadBitmapSurface(src, Width(header), Height(header), size);
        auto rect = MakeRect(header.posX, header.posY, Width(header), Height(header));
        surface->Blit(*buffer, &rect);
    }
};    

template<class EntryClass>
std::shared_ptr<SDLSurface> LoadDrawingPlainEntries(SDL_RWops *src, const GM1CollectionScheme &scheme, Sint64 origin, std::vector<SDL_Rect> &rects)
{
    Uint32 width = 0;
    Uint32 height = 0;
    for(const auto &header : scheme.headers) {
        width = std::max(width, header.posX + EntryClass::Width(header));
        height = std::max(height, header.posY + EntryClass::Height(header));
    }

    Uint32 depth = EntryClass::Depth();
    Uint32 rmask = EntryClass::RedMask();
    Uint32 gmask = EntryClass::GreenMask();
    Uint32 bmask = EntryClass::BlueMask();
    Uint32 amask = EntryClass::AlphaMask();

    std::shared_ptr<SDLSurface> plain =
        std::make_shared<SDLSurface>(
            width, height, depth,
            rmask, gmask, bmask, amask);
    
    for(size_t i = 0; i < GetGM1ImageCount(scheme.header); ++i) {
        SDL_RWseek(src, origin + scheme.offsets[i], RW_SEEK_SET);
        EntryClass::Load(src, scheme.sizes[i], scheme.headers[i], plain);
        rects.push_back(
            MakeRect(
                scheme.headers[i].posX,
                scheme.headers[i].posY,
                EntryClass::Width(scheme.headers[i]),
                EntryClass::Height(scheme.headers[i])));
    }
    
    return plain;
}

std::shared_ptr<SDLSurface> LoadDrawingPlain(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<SDL_Rect> &rects)
{
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    size_t count = GetGM1ImageCount(scheme.header);

    Uint32 lastByte = 0;
    for(size_t i = 0; i < count; ++i) {
        lastByte = std::max(lastByte, scheme.offsets[i] + scheme.sizes[i]);
    }
    
    Sint64 size = SDL_RWsize(src);
    if(origin + lastByte > size) {
        SDL_Log("Last byte found at %d, but there is EOF at %d",
                static_cast<int>(origin + lastByte),
                static_cast<int>(size));
        throw EOFError("Check eof failed");
    }

    ImageEncoding encoding = GetGM1ImageEncoding(scheme.header);
    switch(encoding) {
    case ImageEncoding::Bitmap:
        return LoadDrawingPlainEntries<Bitmap>(src, scheme, origin, rects);
    case ImageEncoding::TGX16:
        return LoadDrawingPlainEntries<TGX16>(src, scheme, origin, rects);
    case ImageEncoding::TGX8:
        return LoadDrawingPlainEntries<TGX8>(src, scheme, origin, rects);
    case ImageEncoding::TileObject:
        return LoadDrawingPlainEntries<TileObject>(src, scheme, origin, rects);
    default:
        throw FormatError("Unknown encoding");
    }
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

std::shared_ptr<SDLSurface> LoadEntrySurface(SDL_RWops *src, const GM1Header &gm1, const GM1ImageHeader &header, Sint64 size)
{
    switch(GetGM1ImageEncoding(gm1)) {
    case ImageEncoding::Bitmap:
        // I don't know why: header.height - 7
        return LoadBitmapSurface(src, header.width, header.height - 7, size);
        break;
    case ImageEncoding::TGX16:
        return LoadTGX16Surface(src, header.width, header.height, size);
        break;
    case ImageEncoding::TGX8:
        return LoadTGX8Surface(src, header.width, header.height, size);
        break;
    case ImageEncoding::TileObject:
        return LoadTileObjectSurface(src, header, size);
        break;
    default:
        throw FormatError("Unknown encoding");
    }
}

void LoadEntries(SDL_RWops *src, const GM1CollectionScheme &scheme, std::vector<std::shared_ptr<SDLSurface>> &entries)
{
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);

    auto count = GetGM1ImageCount(scheme.header);
    entries.resize(count);
    
    Uint32 lastByte = 0;
    for(size_t index = 0; index < count; ++index) {
        lastByte = std::max(lastByte, scheme.offsets[index] + scheme.sizes[index]);
    }

    Uint32 size = SDL_RWsize(src);
    if(origin + lastByte > size) {
        SDL_Log("Last byte found at %d, but there is EOF at %d",
                origin + lastByte,
                size);
        throw EOFError("Check eof failed");
    }
    
    for(size_t index = 0; index < count; ++index) {
        SDL_RWseek(src, origin + scheme.offsets[index], RW_SEEK_SET);
        entries[index] = LoadEntrySurface(
            src,
            scheme.header,
            scheme.headers[index],
            scheme.sizes[index]);
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
    SDL_Rect tilerect;
    tilerect.x = 0;
    tilerect.y = header.tileY;
    tilerect.w = TILE_RHOMBUS_WIDTH;
    tilerect.h = TILE_RHOMBUS_HEIGHT;
    surface->Blit(*tile, &tilerect);
    
    auto box = LoadTGX16Surface(
        src, header.boxWidth, height, size - TILE_BYTES);
    SDL_Rect boxrect;
    boxrect.x = header.hOffset;
    boxrect.y = 0;
    boxrect.w = header.boxWidth;
    boxrect.h = height;
    surface->Blit(*box, &boxrect);

    return surface;
}

static bool ReadIntSequenceLE32(SDL_RWops *src, Uint32 *data, Uint32 count)
{
    if(!CheckBytesAvailable(src, count * sizeof(Uint32)))
        return false;
    for(size_t i = 0; i < count; ++i)
        data[i] = SDL_ReadLE32(src);
    return true;
}

static bool ReadIntSequenceLE16(SDL_RWops *src, Uint16 *data, Uint32 count)
{
    if(!CheckBytesAvailable(src, count * sizeof(Uint16)))
        return false;
    for(size_t i = 0; i < count; ++i)
        data[i] = SDL_ReadLE16(src);
    return true;
}

static void ReadGM1Header(SDL_RWops *src, GM1Header *hdr)
{
    if(!ReadIntSequenceLE32(src, &(*hdr)[0], GM1_HEADER_FIELDS))
        throw EOFError("ReadGM1Header");
}

static void ReadGM1Palette(SDL_RWops *src, GM1Palette *palette)
{
    if(!ReadIntSequenceLE16(src, &(*palette)[0], GM1_PALETTE_COLORS))
        throw EOFError("ReadGM1Palette");
}

static void ReadGM1ImageHeader(SDL_RWops *src, GM1ImageHeader *hdr)
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

static const char * GetGM1ImageClassName(Uint32 cl)
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

static const char* GetGM1HeaderFieldName(size_t i)
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

static void DebugPrint_GM1ImageHeader(const GM1ImageHeader &header)
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

static void DebugPrint_GM1Header(const GM1Header &header)
{
    SDL_Log("ImageCount: %d", GetGM1ImageCount(header));
    SDL_Log("ImageClass: %s", GetGM1ImageClassName(GetGM1ImageClass(header)));
    SDL_Log("ImageSize: %d", GetGM1ImageSize(header));
    for(size_t i = 0; i < GM1_HEADER_FIELDS; ++i) {
        SDL_Log("Field #%d: %d --\t%s", i, header[i], GetGM1HeaderFieldName(i));
    }    
}

static void DebugPrint_GM1Palette(const GM1Palette &palette)
{
    for(size_t i = 0; i < palette.size(); ++i)
        SDL_Log("# %03d:\t%4x", i, palette[i]);
}

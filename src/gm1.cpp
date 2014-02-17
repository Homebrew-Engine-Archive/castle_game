#include "gm1.h"

NAMESPACE_BEGIN(gm1)

static void ReadHeader(SDL_RWops *, Header *);
static void ReadPalette(SDL_RWops *, Palette *);
static void ReadImageHeader(SDL_RWops *, ImageHeader *);
static const char* GetImageClassName(Uint32);
static const char* GetHeaderFieldName(size_t);
static void LoadTGX16Surface(SDL_RWops *, Surface &, Sint64);
static void LoadTGX8Surface(SDL_RWops *, Surface &, Sint64);
static void LoadBitmapSurface(SDL_RWops *, Surface &, Sint64);
static void LoadTileSurface(SDL_RWops *, Surface &);

template<class EntryClass>
static Surface LoadAtlasEntries(SDL_RWops *, const Collection &, Sint64);

template<class EntryClass>
static Surface AllocateSurface(Uint32 width, Uint32 height);

template<class EntryClass>
static void EvalAtlasPartitionFor(const Collection &, std::vector<SDL_Rect> &);

Collection::Collection(SDL_RWops *src)
    throw(GM1Error)
{
    ReadHeader(src, &header);
    if(ReadableBytes(src) < GetImageSize(header))
        throw GM1Error("EOF while GetImageSize");
    Uint32 count = GetImageCount(header);

    palettes.resize(GM1_PALETTE_COUNT);
    for(Palette &palette : palettes)
        ReadPalette(src, &palette);

    if(ReadableBytes(src) < sizeof(Uint32) * count)
        throw GM1Error("EOF while reading offsets");
    offsets.resize(count);
    ReadInt32Array(src, offsets.data(), offsets.size());

    if(ReadableBytes(src) < sizeof(Uint32) * count)
        throw GM1Error("EOF while reading sizes");
    sizes.resize(count);
    ReadInt32Array(src, sizes.data(), sizes.size());

    headers.resize(count);
    for(ImageHeader &header : headers)
        ReadImageHeader(src, &header);
}

const SDL_Palette *CreateSDLPaletteFrom(const Palette &gm1pal)
{
    SDL_Palette *palette = SDL_AllocPalette(GM1_PALETTE_COLORS);
    if(palette == NULL)
        throw SDLError(SDL_GetError());

    std::vector<SDL_Color> colors;
    colors.reserve(GM1_PALETTE_COLORS);
    for(Uint16 color: gm1pal) {
        SDL_Color c;
        c.r = tgx::GetRed(color);
        c.g = tgx::GetGreen(color);
        c.b = tgx::GetBlue(color);
        c.a = tgx::GetAlpha(color);
        colors.push_back(c);
    }
    
    if(SDL_SetPaletteColors(palette, &colors[0], 0, GM1_PALETTE_COLORS)) {
        SDL_FreePalette(palette);
        throw SDLError(SDL_GetError());
    }
    
    return palette;
}

struct TGX8
{
    static Uint32 Width(const ImageHeader &header) {
        return header.width;
    }
    
    static Uint32 Height(const ImageHeader &header) {
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
    
    static Uint32 ColorKey() {
        return GM1_TGX8_TRANSPARENT_INDEX;
    }

    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &header, Surface &atlas) {
        Uint32 width = Width(header);
        Uint32 height = Height(header);
        
        Surface buffer = AllocateSurface<TGX8>(width, height);
        LoadTGX8Surface(src, buffer, size);

        SDL_Rect whither = MakeRect(
            header.posX,
            header.posY,
            width,
            height);
        BlitSurface(buffer, NULL, atlas, &whither);
    }
};

struct TGX16
{
    static Uint32 Width(const ImageHeader &header) {
        return header.width;
    }
    
    static Uint32 Height(const ImageHeader &header) {
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
    
    static Uint32 ColorKey() {
        return TGX_TRANSPARENT_RGB16;
    }
    
    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &header, Surface &surface) {
        Uint32 width = Width(header);
        Uint32 height = Height(header);
        
        Surface buffer = AllocateSurface<TGX16>(width, height);
        LoadTGX16Surface(src, buffer, size);
        
        SDL_Rect whither = MakeRect(
            header.posX,
            header.posY,
            width,
            height);
        BlitSurface(buffer, NULL, surface, &whither);
    }
};

struct TileObject
{
    static Uint32 Width(const ImageHeader &) {
        return TILE_RHOMBUS_WIDTH;
    }
    
    static Uint32 Height(const ImageHeader &header) {
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
    
    static Uint32 ColorKey() {
        return TGX_TRANSPARENT_RGB16;
    }
    
    void Load(SDL_RWops *src, Sint64 size, const ImageHeader &header, Surface &surface) {
        Surface tile = AllocateSurface<TileObject>(
            TILE_RHOMBUS_WIDTH, TILE_RHOMBUS_HEIGHT);
        LoadTileSurface(src, tile);
        
        SDL_Rect tilerect = MakeRect(
            header.posX,
            header.posY + header.tileY,
            Width(header),
            TILE_RHOMBUS_HEIGHT);
        BlitSurface(tile, NULL, surface, &tilerect);
        
        Surface box = AllocateSurface<TGX16>(
            header.boxWidth, Height(header));
        LoadTGX16Surface(src, box, size - TILE_BYTES);
        
        SDL_Rect boxrect = MakeRect(
            header.posX + header.hOffset,
            header.posY,
            header.boxWidth,
            Height(header));
        BlitSurface(box, NULL, surface, &boxrect);
    }
};

struct Bitmap
{
    static Uint32 Width(const ImageHeader &header) {
        return header.width;
    }
    
    static Uint32 Height(const ImageHeader &header) {
        // Nobody knows why
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
    
    static Uint32 ColorKey() {
        return TGX_TRANSPARENT_RGB16;
    }
    
    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &header, Surface &surface) {
        Uint32 width = Width(header);
        Uint32 height = Height(header);
        
        Surface buffer = AllocateSurface<Bitmap>(width, height);
        LoadBitmapSurface(src, buffer, size);
        
        SDL_Rect whither = MakeRect(
            header.posX,
            header.posY,
            width,
            height);
        BlitSurface(buffer, NULL, surface, &whither);
    }
};    

std::vector<SDL_Rect> EvalAtlasPartition(const Collection &gm1)
{
    std::vector<SDL_Rect> rects;
    rects.reserve(GetImageCount(gm1.header));
    
    Encoding encoding = GetEncoding(gm1.header);
    switch(encoding) {
    case Encoding::TGX8:
        EvalAtlasPartitionFor<TGX8>(gm1, rects);
        break;
    case Encoding::TGX16:
        EvalAtlasPartitionFor<TGX16>(gm1, rects);
        break;
    case Encoding::Bitmap:
        EvalAtlasPartitionFor<Bitmap>(gm1, rects);
        break;
    case Encoding::TileObject:
        EvalAtlasPartitionFor<TileObject>(gm1, rects);
        break;
    default:
        throw GM1Error("Unknown encoding");
    }
    
    return rects;
}

template<class EntryClass>
static void EvalAtlasPartitionFor(const Collection &gm1, std::vector<SDL_Rect> &rects)
{
    auto count = GetImageCount(gm1.header);
    for(size_t i = 0; i < count; ++i) {
        auto header = gm1.headers[i];
        rects.push_back(
            MakeRect(
                header.posX,
                header.posY,
                EntryClass::Width(header),
                EntryClass::Height(header)));
    }
}

Surface LoadAtlas(SDL_RWops *src, const Collection &gm1)
    throw(GM1Error, TGXError, SDLError)
{
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    if(origin < 0)
        throw SDLError(SDL_GetError());
    size_t count = GetImageCount(gm1.header);

    // Find collection size
    Uint32 lastByte = 0;
    for(size_t i = 0; i < count; ++i) {
        lastByte = std::max(lastByte, gm1.offsets[i] + gm1.sizes[i]);
    }
    
    if(ReadableBytes(src) < lastByte) {
        Uint32 size = SDL_RWsize(src);
        SDL_Log("Last byte found at %d, but there is EOF at %d",
                static_cast<int>(origin + lastByte),
                static_cast<int>(size));
        throw GM1Error("EOF while LoadAtlas");
    }

    // Dispatch collection reading by image encoding class
    Encoding encoding = GetEncoding(gm1.header);
    switch(encoding) {
    case Encoding::Bitmap:
        return LoadAtlasEntries<Bitmap>(src, gm1, origin);
    case Encoding::TGX16:
        return LoadAtlasEntries<TGX16>(src, gm1, origin);
    case Encoding::TGX8:
        return LoadAtlasEntries<TGX8>(src, gm1, origin);
    case Encoding::TileObject:
        return LoadAtlasEntries<TileObject>(src, gm1, origin);
    default:
        throw GM1Error("Unknown encoding");
    }
}

template<class EntryClass>
static Surface LoadAtlasEntries(SDL_RWops *src, const Collection &gm1, Sint64 origin)
{
    Uint32 width = 0;
    Uint32 height = 0;
    // Eval size for atlas
    for(const auto &header : gm1.headers) {
        width = std::max(width, header.posX + EntryClass::Width(header));
        height = std::max(height, header.posY + EntryClass::Height(header));
    }

    Surface atlas = AllocateSurface<EntryClass>(width, height);

    EntryClass reader;

    // Dispatch reading entries to EntryClass::Load
    for(size_t i = 0; i < GetImageCount(gm1.header); ++i) {
        SDL_RWseek(src, origin + gm1.offsets[i], RW_SEEK_SET);
        //EntryClass::Load(src, gm1.sizes[i], gm1.headers[i], atlas);
        reader.Load(src, gm1.sizes[i], gm1.headers[i], atlas);
    }
    
    return atlas;
}

template<class EntryClass>
static Surface AllocateSurface(Uint32 width, Uint32 height) {
    Surface sf(
        width, height, EntryClass::Depth(),
        EntryClass::RedMask(),
        EntryClass::GreenMask(),
        EntryClass::BlueMask(),
        EntryClass::AlphaMask());    
    SetColorKey(sf, EntryClass::ColorKey());
    FillRect(sf, NULL, EntryClass::ColorKey());
    return sf;
}

static void LoadTGX16Surface(SDL_RWops *src, Surface &surface, Sint64 size)
{
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    tgx::ReadTGX16(src, size, surface->w, surface->h, bits);
}

static void LoadTGX8Surface(SDL_RWops *src, Surface &surface, Sint64 size)
{
    Uint8 *bits = reinterpret_cast<Uint8*>(surface->pixels);
    tgx::ReadTGX8(src, size, surface->w, surface->h, bits);
}

static void LoadBitmapSurface(SDL_RWops *src, Surface &surface, Sint64 size)
{
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    tgx::ReadBitmap(src, size, bits);
}

static void LoadTileSurface(SDL_RWops *src, Surface &surface/*, Sint64 size = TILE_BYTES */)
{
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    tgx::ReadTile(src, bits);
}

static void ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < GM1_HEADER_BYTES)
        throw GM1Error("EOF while ReadHeader");
    ReadInt32Array(src, &(*hdr)[0], GM1_HEADER_FIELDS);
}

static void ReadPalette(SDL_RWops *src, Palette *palette)
{
    if(ReadableBytes(src) < sizeof(Palette))
        throw GM1Error("EOF while ReadPalette");
    
    ReadInt16Array(src, &(*palette)[0], GM1_PALETTE_COLORS);
}

static void ReadImageHeader(SDL_RWops *src, ImageHeader *hdr)
{
    if(ReadableBytes(src) < GM1_IMAGE_HEADER_BYTES)
        throw GM1Error("EOF while ReadImageHeader");
        
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

Encoding GetEncoding(const Header &hdr)
{
    switch(GetImageClass(hdr)) {
    case 1: return Encoding::TGX16;
    case 2: return Encoding::TGX8;
    case 3: return Encoding::TileObject;
    case 4: return Encoding::TGX16;
    case 5: return Encoding::Bitmap;
    case 6: return Encoding::TGX16;
    case 7: return Encoding::Bitmap;
    default: return Encoding::Unknown;
    }
}

static const char * GetImageClassName(Uint32 cl)
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

static const char* GetHeaderFieldName(size_t i)
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

void VerbosePrintImageHeader(const ImageHeader &header)
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

void VerbosePrintHeader(const Header &header)
{
    SDL_Log("ImageCount: %d", GetImageCount(header));
    SDL_Log("ImageClass: %s", GetImageClassName(GetImageClass(header)));
    for(size_t i = 0; i < GM1_HEADER_FIELDS; ++i) {
        SDL_Log("Field #%d: %d --\t%s", i, header[i], GetHeaderFieldName(i));
    }    
}

void VerbosePrintPalette(const Palette &palette)
{
    for(size_t i = 0; i < palette.size(); ++i)
        SDL_Log("# %03d:\t%4x", i, palette[i]);
}

NAMESPACE_END(gm1)

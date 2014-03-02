#include "gm1.h"

NAMESPACE_BEGIN(gm1)

static void ReadHeader(SDL_RWops *src, Header *hdr);
static void ReadPalette(SDL_RWops *src, Palette *palette);
static void ReadImageHeader(SDL_RWops *src, ImageHeader *header);
static const char* GetImageClassName(Uint32 imageClass);
static Encoding GetEncoding(Uint32 dataClass);

template<class EntryClass>
static Surface LoadAtlasEntries(SDL_RWops *, const Collection &, Sint64);

template<class EntryClass>
static Surface AllocateSurface(Uint32 width, Uint32 height);

template<class EntryClass>
static void PartitionEntryClass(const Collection &, std::vector<SDL_Rect> &);

Collection::Collection(SDL_RWops *src)
    throw(std::runtime_error)
{
    ReadHeader(src, &header);
    if(ReadableBytes(src) < header.dataSize)
        throw std::runtime_error("failed comparision against header.dataSize");
    Uint32 count = header.imageCount;

    palettes.resize(GM1_PALETTE_COUNT);
    for(Palette &palette : palettes)
        ReadPalette(src, &palette);

    if(ReadableBytes(src) < sizeof(Uint32) * count)
        throw std::runtime_error("EOF while reading offsets");
    offsets.resize(count);
    ReadInt32ArrayLE(src, offsets.data(), offsets.size());

    if(ReadableBytes(src) < sizeof(Uint32) * count)
        throw std::runtime_error("EOF while reading sizes");
    sizes.resize(count);
    ReadInt32ArrayLE(src, sizes.data(), sizes.size());

    headers.resize(count);
    for(ImageHeader &hdr : headers)
        ReadImageHeader(src, &hdr);
}

Encoding Collection::encoding() const
{
    return GetEncoding(header.dataClass);
}

SDL_Palette *CreateSDLPaletteFrom(const Palette &gm1pal)
{
    SDL_Palette *palette = SDL_AllocPalette(GM1_PALETTE_COLORS);
    if(palette == NULL) {
        std::cerr << "SDL_AllocPalette failed: ";
        std::cerr << SDL_GetError();
        std::cerr << std::endl;
        return NULL;
    }

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
    
    if(SDL_SetPaletteColors(palette, &colors[0], 0, GM1_PALETTE_COLORS) < 0) {
        SDL_FreePalette(palette);
        std::cerr << "SDL_SetPaletteColors failed: ";
        std::cerr << SDL_GetError();
        std::cerr << std::endl;
        return NULL;
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
        return TGX_TRANSPARENT_RGB8;
    }

    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &header, Surface &atlas) {
        Uint32 width = Width(header);
        Uint32 height = Height(header);
        
        Surface buffer = AllocateSurface<TGX8>(width, height);
        if(buffer.Null()) {
            std::cerr << "TGX8::Load failed" << std::endl;
            return;
        }
        tgx::DecodeTGX(src, size, buffer);

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
        if(buffer.Null()) {
            std::cerr << "TGX16::Load failed" << std::endl;
            return;
        }
        tgx::DecodeTGX(src, size, buffer);
        
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

    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &header, Surface &surface) {
        Surface tile = AllocateSurface<TileObject>(
            TILE_RHOMBUS_WIDTH, TILE_RHOMBUS_HEIGHT);
        if(tile.Null()) {
            std::cerr << "TileObject::Load failed" << std::endl;
            return;
        }
        tgx::DecodeTile(src, TILE_BYTES, tile);
        
        SDL_Rect tilerect = MakeRect(
            header.posX,
            header.posY + header.tileY,
            Width(header),
            TILE_RHOMBUS_HEIGHT);
        BlitSurface(tile, NULL, surface, &tilerect);
        
        Surface box = AllocateSurface<TGX16>(
            header.boxWidth, Height(header));
        if(box.Null()) {
            std::cerr << "TileObject::Load failed" << std::endl;
            return;
        }
        tgx::DecodeTGX(src, size - TILE_BYTES, box);
        
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
        if(buffer.Null()) {
            std::cerr << "Bitmap::Load failed" << std::endl;
            return;
        }
        tgx::DecodeUncompressed(src, size, buffer);
        
        SDL_Rect whither = MakeRect(
            header.posX,
            header.posY,
            width,
            height);
        BlitSurface(buffer, NULL, surface, &whither);
    }
};    

Surface LoadAtlas(SDL_RWops *src, const Collection &gm1)
    throw(std::runtime_error)
{
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    if(origin < 0)
        throw std::runtime_error(SDL_GetError());
    size_t count = gm1.header.imageCount;

    // Find collection size
    Uint32 lastByte = 0;
    for(size_t i = 0; i < count; ++i) {
        lastByte = std::max(lastByte, gm1.offsets[i] + gm1.sizes[i]);
    }
    
    if(ReadableBytes(src) < lastByte) {
        Uint32 size = SDL_RWsize(src);
        std::cerr << "Checking file size: ";
        std::cerr << "eof expected at: " << origin + lastByte;
        std::cerr << ", ";
        std::cerr << "found at: " << size;
        std::cerr << std::endl;
        return Surface();
    }

    // Dispatch collection reading by image encoding class
    Encoding encoding = gm1.encoding();
    switch(encoding) {
    case Encoding::TGX8:
        return LoadAtlasEntries<TGX8>(src, gm1, origin);
    case Encoding::TGX16:
        return LoadAtlasEntries<TGX16>(src, gm1, origin);
    case Encoding::Bitmap:
        return LoadAtlasEntries<Bitmap>(src, gm1, origin);
    case Encoding::TileObject:
        return LoadAtlasEntries<TileObject>(src, gm1, origin);
    default:
        std::cerr << "Unknown encoding" << std::endl;
        return Surface();
    }
}

template<class EntryClass>
static Surface LoadAtlasEntries(SDL_RWops *src, const Collection &gm1, Sint64 origin)
{
    Uint32 width = 0;
    Uint32 height = 0;
    // Eval size for atlas
    for(const ImageHeader &header : gm1.headers) {
        width = std::max(width, header.posX + EntryClass::Width(header));
        height = std::max(height, header.posY + EntryClass::Height(header));
    }

    Surface atlas = AllocateSurface<EntryClass>(width, height);
    if(atlas.Null()) {
        std::cerr << "LoadAtlasEntries failed" << std::endl;
        return Surface();
    }

    // Dispatch reading entries to EntryClass::Load
    for(size_t i = 0; i < gm1.header.imageCount; ++i) {
        SDL_RWseek(src, origin + gm1.offsets[i], RW_SEEK_SET);
        EntryClass::Load(src, gm1.sizes[i], gm1.headers[i], atlas);
    }
    
    return atlas;
}

void PartitionAtlas(const Collection &gm1, std::vector<SDL_Rect> &rects)
{
    rects.reserve(rects.size() + gm1.header.imageCount);
    
    Encoding encoding = gm1.encoding();
    switch(encoding) {
    case Encoding::TGX8:
        PartitionEntryClass<TGX8>(gm1, rects);
        break;
    case Encoding::TGX16:
        PartitionEntryClass<TGX16>(gm1, rects);
        break;
    case Encoding::Bitmap:
        PartitionEntryClass<Bitmap>(gm1, rects);
        break;
    case Encoding::TileObject:
        PartitionEntryClass<TileObject>(gm1, rects);
        break;
    default:
        throw std::runtime_error("Unknown encoding");
    }
}

template<class EntryClass>
static void PartitionEntryClass(const Collection &gm1, std::vector<SDL_Rect> &rects)
{
    Uint32 count = gm1.header.imageCount;
    for(size_t i = 0; i < count; ++i) {
        const ImageHeader &header = gm1.headers[i];
        const SDL_Rect rect = MakeRect(
            header.posX, header.posY,
            EntryClass::Width(header),
            EntryClass::Height(header));
        rects.push_back(rect);
    }
}

template<class EntryClass>
static Surface AllocateSurface(Uint32 width, Uint32 height)
{
    Surface sf = SDL_CreateRGBSurface(
        0, width, height, EntryClass::Depth(),
        EntryClass::RedMask(),
        EntryClass::GreenMask(),
        EntryClass::BlueMask(),
        EntryClass::AlphaMask());
    
    if(sf.Null()) {
        std::cerr << "SDL_CreateRGBSurface failed: ";
        std::cerr << SDL_GetError();
        std::cerr << std::endl;
        return Surface();
    }

    SDL_SetColorKey(sf, SDL_TRUE, EntryClass::ColorKey());
    SDL_FillRect(sf, NULL, EntryClass::ColorKey());
    return sf;
}

template<class EntryClass>
static SDL_Rect GetRectOnAtlas(const ImageHeader &header)
{
    return MakeRect(
        header.posX,
        header.posY,
        EntryClass::Width(header),
        EntryClass::Heigth(header));
}

static void ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < GM1_HEADER_BYTES)
        throw std::runtime_error("EOF while ReadHeader");
    
    hdr->u1             = SDL_ReadLE32(src);
    hdr->u2             = SDL_ReadLE32(src);
    hdr->u3             = SDL_ReadLE32(src);
    hdr->imageCount     = SDL_ReadLE32(src);
    hdr->u4             = SDL_ReadLE32(src);
    hdr->dataClass      = SDL_ReadLE32(src);
    hdr->u5             = SDL_ReadLE32(src);
    hdr->u6             = SDL_ReadLE32(src);
    hdr->sizeCategory   = SDL_ReadLE32(src);
    hdr->u7             = SDL_ReadLE32(src);
    hdr->u8             = SDL_ReadLE32(src);
    hdr->u9             = SDL_ReadLE32(src);
    hdr->width          = SDL_ReadLE32(src);
    hdr->height         = SDL_ReadLE32(src);
    hdr->u10            = SDL_ReadLE32(src);
    hdr->u11            = SDL_ReadLE32(src);
    hdr->u12            = SDL_ReadLE32(src);
    hdr->u13            = SDL_ReadLE32(src);
    hdr->anchorX        = SDL_ReadLE32(src);
    hdr->anchorY        = SDL_ReadLE32(src);
    hdr->dataSize       = SDL_ReadLE32(src);
    hdr->u14            = SDL_ReadLE32(src);
}

static void ReadPalette(SDL_RWops *src, Palette *palette)
{
    if(ReadableBytes(src) < sizeof(Palette))
        throw std::runtime_error("EOF while ReadPalette");
    
    ReadInt16ArrayLE(src, &(*palette)[0], GM1_PALETTE_COLORS);
}

static void ReadImageHeader(SDL_RWops *src, ImageHeader *hdr)
{
    if(ReadableBytes(src) < GM1_IMAGE_HEADER_BYTES)
        throw std::runtime_error("EOF while ReadImageHeader");
        
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

Encoding GetEncoding(Uint32 dataClass)
{
    switch(dataClass) {
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

static const char * GetImageClassName(Uint32 dataClass)
{
    MAKE_COMPILER_HAPPY(GetImageClassName);
    switch(dataClass) {
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

void VerbosePrintImageHeader(const ImageHeader &header)
{
    std::clog << "Width: " << header.width << std::endl;
    std::clog << "Height: " << header.height << std::endl;
    std::clog << "PosX: " << header.posX << std::endl;
    std::clog << "PosY: " << header.posY << std::endl;
    std::clog << "Group: " << header.group << std::endl;
    std::clog << "GroupSize: " << header.groupSize << std::endl;
    std::clog << "TileY: " << header.tileY << std::endl;
    std::clog << "TileOrient: " << header.tileOrient << std::endl;
    std::clog << "Horizontal Offset: " << header.hOffset << std::endl;
    std::clog << "Box Width: " << header.boxWidth << std::endl;
    std::clog << "Flags: " << header.flags << std::endl;
}

void VerbosePrintHeader(const Header &header)
{
    std::clog << "u1: " << header.u1 << std::endl;
    std::clog << "u2: " << header.u2 << std::endl;
    std::clog << "u3: " << header.u3 << std::endl;
    std::clog << "imageCount: " << header.imageCount << std::endl;
    std::clog << "u4: " << header.u4 << std::endl;
    std::clog << "dataClass: " << header.dataClass << std::endl;
    std::clog << "u5: " << header.u5 << std::endl;
    std::clog << "u6: " << header.u6 << std::endl;
    std::clog << "sizeCategory: " << header.sizeCategory << std::endl;
    std::clog << "u7: " << header.u7 << std::endl;
    std::clog << "u8: " << header.u8 << std::endl;
    std::clog << "u9: " << header.u9 << std::endl;
    std::clog << "width: " << header.width << std::endl;
    std::clog << "height: " << header.height << std::endl;
    std::clog << "u10: " << header.u10 << std::endl;
    std::clog << "u11: " << header.u11 << std::endl;
    std::clog << "u12: " << header.u12 << std::endl;
    std::clog << "u13: " << header.u13 << std::endl;
    std::clog << "anchorX: " << header.anchorX << std::endl;
    std::clog << "anchorY: " << header.anchorY << std::endl;
    std::clog << "dataSize: " << header.dataSize << std::endl;
    std::clog << "u14: " << header.u14 << std::endl;
}

void VerbosePrintPalette(const Palette &palette)
{
    for(size_t i = 0; i < palette.size(); ++i) {
        std::clog << std::hex;
        for(size_t j = 0; j < 16; ++j, ++i) {
            std::clog << palette[i] << ' ';
        }
        std::clog << std::endl;
    }
}

void VerbosePrintCollection(const Collection &gm1)
{
    std::clog << "--- GM1 header ---" << std::endl;
    VerbosePrintHeader(gm1.header);
    std::clog << "--- end GM1 header ---" << std::endl;
    for(size_t index = 0; index < gm1.palettes.size(); ++index) {
        std::clog << "--- GM1 Palette # " << index << std::endl;
        VerbosePrintPalette(gm1.palettes[index]);
        std::clog << "--- end GM1 Palette # " << index << std::endl;
    }
    for(size_t i = 0; i < gm1.offsets.size(); ++i) {
        std::clog << "Offset #" << i << ": " << gm1.offsets[i] << std::endl;
    }
    for(size_t i = 0; i < gm1.sizes.size(); ++i) {
        std::clog << "Size #" << i << ": " << gm1.sizes[i] << std::endl;
    }
    for(size_t i = 0; i < gm1.headers.size(); ++i) {
        std::clog << "--- Image header # " <<  i << std::endl;
        VerbosePrintImageHeader(gm1.headers[i]);
        std::clog << "--- end image header # " << i << std::endl;
    }
    Sint64 origin = GM1_HEADER_BYTES
        + GM1_IMAGE_HEADER_BYTES * gm1.header.imageCount
        + GM1_PALETTE_COLORS * GM1_PALETTE_COUNT * sizeof(Uint16)
        + sizeof(Uint32) * gm1.header.imageCount
        + sizeof(Uint32) * gm1.header.imageCount;
    std::clog << "Data entry point at " << origin << std::endl;
}

NAMESPACE_END(gm1)

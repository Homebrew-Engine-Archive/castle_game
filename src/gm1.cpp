#include "gm1.h"

NAMESPACE_BEGIN(gm1)

// EntryClass
struct TGX16;
struct TGX8;
struct TileObject;
struct Bitmap;

static void ReadHeader(SDL_RWops *src, Header *hdr);
static void ReadPalette(SDL_RWops *src, Palette *palette);
static void ReadImageHeader(SDL_RWops *src, ImageHeader *header);
static const char* GetImageClassName(Uint32 imageClass);
static Encoding GetEncoding(Uint32 dataClass);

template<class EntryClass>
static SDL_Rect GetRectOnAtlas(const ImageHeader &header);

template<class EntryClass>
static Surface AllocateSurface(Uint32 width, Uint32 height);

template<class EntryClass>
static Surface LoadAtlasImpl(SDL_RWops *, const Collection &);

template<class EntryClass>
static void LoadEntriesImpl(SDL_RWops *src, const Collection &gm1, std::vector<Surface> &atlas);

template<class EntryClass>
static void PartitionAtlasImpl(const Collection &, std::vector<SDL_Rect> &);

Collection::Collection(SDL_RWops *src)
    throw(std::runtime_error)
{
    if(src == NULL)
        throw std::runtime_error("passed NULL file");
    
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

size_t Collection::size() const
{
    return header.imageCount;
}

PalettePtr CreateSDLPaletteFrom(const Palette &gm1pal)
{
    PalettePtr ptr =
        PalettePtr(
            SDL_AllocPalette(GM1_PALETTE_COLORS));
    if(!ptr) {
        std::cerr << "SDL_AllocPalette failed: "
                  << SDL_GetError()
                  << std::endl;
        return PalettePtr(nullptr);
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
    
    if(SDL_SetPaletteColors(ptr.get(), &colors[0], 0, GM1_PALETTE_COLORS) < 0) {
        std::cerr << "SDL_SetPaletteColors failed: "
                  << SDL_GetError()
                  << std::endl;
        return PalettePtr(nullptr);
    }
    
    return ptr;
}

Surface LoadAtlas(SDL_RWops *src, const Collection &gm1)
    throw(std::runtime_error)
{
    if(src == NULL)
        return Surface();
    
    // Approximate collection size
    Uint32 lastByte = 0;
    for(size_t i = 0; i < gm1.size(); ++i) {
        lastByte = std::max(lastByte, gm1.offsets[i] + gm1.sizes[i]);
    }
    
    if(ReadableBytes(src) < lastByte) {
        std::cerr << "Checking file size failed: " << std::endl
                  << "MaxOffset=" << lastByte
                  << std::endl;
        return Surface();
    }

    // Dispatch collection reading by image encoding class
    Encoding encoding = gm1.encoding();
    switch(encoding) {
    case Encoding::TGX8:
        return LoadAtlasImpl<TGX8>(src, gm1);
    case Encoding::Font:
        /* fallthrough */
    case Encoding::TGX16:
        return LoadAtlasImpl<TGX16>(src, gm1);
    case Encoding::Bitmap:
        return LoadAtlasImpl<Bitmap>(src, gm1);
    case Encoding::TileObject:
        return LoadAtlasImpl<TileObject>(src, gm1);
    default:
        std::cerr << "Unknown encoding" << std::endl;
        return Surface();
    }
}

void PartitionAtlas(const Collection &gm1, std::vector<SDL_Rect> &rects)
{
    rects.reserve(gm1.size());
    
    Encoding encoding = gm1.encoding();
    switch(encoding) {
    case Encoding::TGX8:
        PartitionAtlasImpl<TGX8>(gm1, rects);
        break;
    case Encoding::Font:
        /* fallthrough */
    case Encoding::TGX16:
        PartitionAtlasImpl<TGX16>(gm1, rects);
        break;
    case Encoding::Bitmap:
        PartitionAtlasImpl<Bitmap>(gm1, rects);
        break;
    case Encoding::TileObject:
        PartitionAtlasImpl<TileObject>(gm1, rects);
        break;
    default:
        throw std::runtime_error("Unknown encoding");
    }
}

void LoadEntries(SDL_RWops *src, const Collection &scheme, std::vector<Surface> &atlas)
{
    switch(scheme.encoding()) {
    case Encoding::TileObject:
        LoadEntriesImpl<TileObject>(src, scheme, atlas);
        break;
    case Encoding::Bitmap:
        LoadEntriesImpl<Bitmap>(src, scheme, atlas);
        break;
    case Encoding::Font:
        /* fallthrough */
    case Encoding::TGX16:
        LoadEntriesImpl<TGX16>(src, scheme, atlas);
        break;
    case Encoding::TGX8:
        LoadEntriesImpl<TGX8>(src, scheme, atlas);
        break;
    default:
        throw std::runtime_error("Unsupported encoding");
    }
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

    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &, Surface &surface) {
        if(tgx::DecodeTGX(src, size, surface)) {
            std::cerr << "TGX8::Load failed" << std::endl;
        }
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
    
    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &, Surface &surface) {
        if(tgx::DecodeTGX(src, size, surface)) {
            std::cerr << "TGX16::Load failed" << std::endl;
        }
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
        SDL_Rect tilerect = MakeRect(
            0,
            header.tileY,
            Width(header),
            TILE_RHOMBUS_HEIGHT);
        SurfaceROI tile(surface, &tilerect);
        if(tgx::DecodeTile(src, TILE_BYTES, tile)) {
            std::cerr << "TileObject::Load failed" << std::endl;
        }
        
        SDL_Rect boxrect = MakeRect(
            header.hOffset,
            0,
            header.boxWidth,
            Height(header));
        SurfaceROI box(surface, &boxrect);
        if(tgx::DecodeTGX(src, size - TILE_BYTES, box)) {
            std::cerr << "TileObject::Load failed" << std::endl;
        }
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
    
    static void Load(SDL_RWops *src, Sint64 size, const ImageHeader &, Surface &surface) {
        if(tgx::DecodeUncompressed(src, size, surface)) {
            std::cerr << "Bitmap::Load failed" << std::endl;
        }
    }
};    

template<class EntryClass>
static SDL_Rect GetRectOnAtlas(const ImageHeader &header)
{
    return MakeRect(
        header.posX,
        header.posY,
        EntryClass::Width(header),
        EntryClass::Height(header));
}

template<class EntryClass>
static Surface AllocateSurface(Uint32 width, Uint32 height)
{
    Surface sf = SDL_CreateRGBSurface(
        NO_FLAGS, width, height, EntryClass::Depth(),
        EntryClass::RedMask(),
        EntryClass::GreenMask(),
        EntryClass::BlueMask(),
        EntryClass::AlphaMask());
    
    if(sf.Null()) {
        std::cerr << "SDL_CreateRGBSurface failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }

    SDL_SetColorKey(sf, SDL_TRUE, EntryClass::ColorKey());
    SDL_FillRect(sf, NULL, EntryClass::ColorKey());
    return sf;
}

template<class EntryClass>
static Surface LoadAtlasImpl(SDL_RWops *src, const Collection &gm1)
{
    Sint64 origin = SDL_RWtell(src);
    if(origin < 0) {
        std::cerr << "SDL_RWtell failed: "
                  << SDL_GetError()
                  << std::endl;
        throw std::runtime_error(SDL_GetError());
    }

    // Eval size for atlas (just convex hull)
    int boundWidth = 0;
    int boundHeight = 0;
    for(const ImageHeader &header : gm1.headers) {
        boundWidth = std::max(boundWidth, header.posX + header.width);
        boundHeight = std::max(boundHeight, header.posY + header.height);
    }
    
    Surface atlas = AllocateSurface<EntryClass>(boundWidth, boundHeight);
    if(atlas.Null()) {
        std::cerr << "LoadAtlasEntries failed" << std::endl;
        return Surface();
    }

    for(size_t i = 0; i < gm1.size(); ++i) {
        SDL_RWseek(src, origin + gm1.offsets[i], RW_SEEK_SET);
        ImageHeader header = gm1.headers[i];
        SDL_Rect roirect = GetRectOnAtlas<EntryClass>(header);
        SurfaceROI roi(atlas, &roirect);
        EntryClass::Load(src, gm1.sizes[i], header, roi);
    }
    
    return atlas;
}

template<class EntryClass>
static void PartitionAtlasImpl(const Collection &gm1, std::vector<SDL_Rect> &rects)
{
    size_t count = gm1.size();
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
static void LoadEntriesImpl(SDL_RWops *src, const Collection &gm1, std::vector<Surface> &atlas)
{
    Sint64 origin = SDL_RWtell(src);
    atlas.reserve(gm1.size());
    for(size_t i = 0; i < gm1.size(); ++i) {
        const ImageHeader &header = gm1.headers.at(i);
        Surface surface = AllocateSurface<EntryClass>(
            EntryClass::Width(header),
            EntryClass::Height(header));
        if(surface.Null())
            throw std::runtime_error("Surface is null");
        SDL_RWseek(src, origin + gm1.offsets[i], RW_SEEK_SET);
        EntryClass::Load(src, gm1.sizes[i], header, surface);
        atlas.push_back(surface);
    }
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
    case 4: return Encoding::Font;
    case 5: return Encoding::Bitmap;
    case 6: return Encoding::TGX16;
    case 7: return Encoding::Bitmap;
    default: return Encoding::Unknown;
    }
}

static const char * GetImageClassName(Uint32 dataClass)
{
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
    using namespace std;
    clog << "Width: "            << (int)header.width << endl
         << "Height: "           << (int)header.height << endl
         << "PosX: "             << (int)header.posX << endl
         << "PosY: "             << (int)header.posY << endl
         << "Group: "            << (int)header.group << endl
         << "GroupSize: "        << (int)header.groupSize << endl
         << "TileY: "            << (int)header.tileY << endl
         << "TileOrient: "       << (int)header.tileOrient << endl
         << "Horiz Offset: "     << (int)header.hOffset << endl
         << "Box Width: "        << (int)header.boxWidth << endl
         << "Flags: "            << (int)header.flags << endl;
}

void VerbosePrintHeader(const Header &header)
{
    using namespace std;
    clog << "u1: "               << header.u1 << endl
         << "u2: "               << header.u2 << endl
         << "u3: "               << header.u3 << endl
         << "imageCount: "       << header.imageCount << endl
         << "u4: "               << header.u4 << endl
         << "dataClass: "        << GetImageClassName(header.dataClass) << endl
         << "u5: "               << header.u5 << endl
         << "u6: "               << header.u6 << endl
         << "sizeCategory: "     << header.sizeCategory << endl
         << "u7: "               << header.u7 << endl
         << "u8: "               << header.u8 << endl
         << "u9: "               << header.u9 << endl
         << "width: "            << header.width << endl
         << "height: "           << header.height << endl
         << "u10: "              << header.u10 << endl
         << "u11: "              << header.u11 << endl
         << "u12: "              << header.u12 << endl
         << "u13: "              << header.u13 << endl
         << "anchorX: "          << header.anchorX << endl
         << "anchorY: "          << header.anchorY << endl
         << "dataSize: "         << header.dataSize << endl
         << "u14: "              << header.u14 << endl;
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
    VerbosePrintHeader(gm1.header);
    for(size_t index = 0; index < gm1.palettes.size(); ++index) {
        std::clog << "palette " << index << std::endl;
        VerbosePrintPalette(gm1.palettes[index]);
    }
    for(size_t i = 0; i < gm1.offsets.size(); ++i) {
        std::clog << "Offset #" << i << ": " << gm1.offsets[i] << std::endl;
    }
    for(size_t i = 0; i < gm1.sizes.size(); ++i) {
        std::clog << "Size #" << i << ": " << gm1.sizes[i] << std::endl;
    }
    for(size_t i = 0; i < gm1.headers.size(); ++i) {
        std::clog << "image header " << i << std::endl;
        VerbosePrintImageHeader(gm1.headers[i]);
    }
    Sint64 origin = GM1_HEADER_BYTES
        + GM1_IMAGE_HEADER_BYTES * gm1.size()
        + GM1_PALETTE_COLORS * GM1_PALETTE_COUNT * sizeof(Uint16)
        + sizeof(Uint32) * gm1.size()
        + sizeof(Uint32) * gm1.size();
    std::clog << "Data entry point at " << origin << std::endl;
}

NAMESPACE_END(gm1)

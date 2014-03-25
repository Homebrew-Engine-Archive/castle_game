#include "gm1.h"
#include "geometry.h"
#include "rw.h"
#include "tgx.h"
#include "surface.h"
#include <string>
#include <iostream>
#include <sstream>

namespace
{
    
    using namespace GM;

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
    
    Encoding GetEncoding(uint32_t dataClass)
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

    std::string GetImageClassName(uint32_t dataClass)
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

    template<class Entry>
    SDL_Rect GetRectOnAtlas(const ImageHeader &header)
    {
        return MakeRect(
            header.posX,
            header.posY,
            Entry::Width(header),
            Entry::Height(header));
    }

    template<class Entry>
    Surface AllocateSurface(int width, int height)
    {
        Surface surface = SDL_CreateRGBSurface(
            NoFlags, width, height, Entry::Depth(),
            Entry::RedMask(),
            Entry::GreenMask(),
            Entry::BlueMask(),
            Entry::AlphaMask());
    
        if(surface.Null()) {
            Fail(__FUNCTION__, SDL_GetError());
        }

        SDL_SetColorKey(surface, SDL_TRUE, Entry::ColorKey());
        SDL_FillRect(surface, NULL, Entry::ColorKey());
        
        return surface;
    }

    template<class Entry>
    SDL_Rect PartitionAtlasImpl(const Collection &gm1, std::vector<SDL_Rect> &rects)
    {
        rects.reserve(gm1.size());
        SDL_Rect bounds = MakeEmptyRect();
        
        for(const ImageHeader &header : gm1.headers) {
            SDL_Rect rect = GetRectOnAtlas<Entry>(header);
            SDL_UnionRect(&rect, &bounds, &bounds);
            rects.push_back(rect);
        }
        
        return bounds;
    }

    template<class Entry>
    Surface LoadAtlasImpl(SDL_RWops *src, const Collection &gm1)
    {
        if(src == NULL)
            return Surface();
    
        // Approximate collection size
        uint32_t lastByte = 0;
        for(size_t i = 0; i < gm1.size(); ++i) {
            lastByte = std::max(lastByte, gm1.offsets[i] + gm1.sizes[i]);
        }
    
        if(ReadableBytes(src) < lastByte) {
            Fail(__FUNCTION__, "Inconsistent size of file");
        }

        int64_t origin = SDL_RWtell(src);
        if(origin < 0) {
            Fail(__FUNCTION__, SDL_GetError());
        }

        std::vector<SDL_Rect> partition;
        SDL_Rect bounds = PartitionAtlasImpl<Entry>(gm1, partition);

        // TODO fix little overhead with bounds.x and bounds.y > 0
        Surface atlas = AllocateSurface<Entry>(
            bounds.x + bounds.w,
            bounds.y + bounds.h);
        
        if(!atlas.Null()) {
            for(size_t i = 0; i < gm1.size(); ++i) {
                SDL_RWseek(src, origin + gm1.offsets[i], RW_SEEK_SET);
                ImageHeader header = gm1.headers[i];
                SurfaceROI roi(atlas, &partition[i]);
                Entry::Load(src, gm1.sizes[i], header, roi);
            }
        } else {
            Fail(__FUNCTION__, "Unable to allocate surface");
        }
        
        return atlas;
    }

    template<class Entry>
    int LoadEntriesImpl(SDL_RWops *src, const Collection &gm1, std::vector<Surface> &atlas)
    {
        atlas.reserve(gm1.size());
        int successfullLoads = 0;
        int64_t origin = SDL_RWtell(src);
        
        for(size_t i = 0; i < gm1.size(); ++i) {
            const ImageHeader &header = gm1.headers.at(i);
            
            Surface surface = AllocateSurface<Entry>(
                Entry::Width(header),
                Entry::Height(header));
            
            if(!surface.Null()) {
                SDL_RWseek(src, origin + gm1.offsets[i], RW_SEEK_SET);
                Entry::Load(src, gm1.sizes[i], header, surface);
                atlas.push_back(surface);
                ++successfullLoads;
            } else {
                Fail(__FUNCTION__, "Unable to allocate surface");
            }
        }

        return successfullLoads;
    }

    template<class Entry>
    Surface LoadEntryImpl(SDL_RWops *src, const Collection &gm1, size_t index)
    {
        int64_t origin = SDL_RWtell(src);
        uint32_t size = gm1.sizes.at(index);
        uint32_t offset = gm1.offsets.at(index);
        const ImageHeader &header = gm1.headers.at(index);

        Surface surface = AllocateSurface<Entry>(
            Entry::Width(header),
            Entry::Height(header));
        
        if(!surface.Null()) {
            TempSeek seekLock(src, origin + offset, RW_SEEK_SET);
            Entry::Load(src, size, header, surface);
        } else {
            Fail(__FUNCTION__, "Unable to allocate surface");
        }

        return surface;
    }
    
    Header ReadHeader(SDL_RWops *src)
    {
        Header hdr;
        hdr.u1             = SDL_ReadLE32(src);
        hdr.u2             = SDL_ReadLE32(src);
        hdr.u3             = SDL_ReadLE32(src);
        hdr.imageCount     = SDL_ReadLE32(src);
        hdr.u4             = SDL_ReadLE32(src);
        hdr.dataClass      = SDL_ReadLE32(src);
        hdr.u5             = SDL_ReadLE32(src);
        hdr.u6             = SDL_ReadLE32(src);
        hdr.sizeCategory   = SDL_ReadLE32(src);
        hdr.u7             = SDL_ReadLE32(src);
        hdr.u8             = SDL_ReadLE32(src);
        hdr.u9             = SDL_ReadLE32(src);
        hdr.width          = SDL_ReadLE32(src);
        hdr.height         = SDL_ReadLE32(src);
        hdr.u10            = SDL_ReadLE32(src);
        hdr.u11            = SDL_ReadLE32(src);
        hdr.u12            = SDL_ReadLE32(src);
        hdr.u13            = SDL_ReadLE32(src);
        hdr.anchorX        = SDL_ReadLE32(src);
        hdr.anchorY        = SDL_ReadLE32(src);
        hdr.dataSize       = SDL_ReadLE32(src);
        hdr.u14            = SDL_ReadLE32(src);
        return hdr;
    }

    Palette ReadPalette(SDL_RWops *src)
    {
        Palette palette;
        ReadInt16ArrayLE(src, &palette[0], CollectionPaletteColors);
        return palette;
    }

    ImageHeader ReadImageHeader(SDL_RWops *src)
    {
        ImageHeader hdr;
        hdr.width      = SDL_ReadLE16(src);
        hdr.height     = SDL_ReadLE16(src);
        hdr.posX       = SDL_ReadLE16(src);
        hdr.posY       = SDL_ReadLE16(src);
        hdr.group      = SDL_ReadU8(src);
        hdr.groupSize  = SDL_ReadU8(src);
        hdr.tileY      = SDL_ReadLE16(src);
        hdr.tileOrient = SDL_ReadU8(src);
        hdr.hOffset    = SDL_ReadU8(src);
        hdr.boxWidth   = SDL_ReadU8(src);
        hdr.flags      = SDL_ReadU8(src);
        return hdr;
    }

    struct TGX8
    {
        static int Width(const ImageHeader &header) {
            return header.width;
        }
        static int Height(const ImageHeader &header) {
            return header.height;
        }
        static int Depth() {
            return 8;
        }
        static uint32_t RedMask() {
            return DefaultRedMask;
        }
        static uint32_t GreenMask() {
            return DefaultGreenMask;
        }
        static uint32_t BlueMask() {
            return DefaultBlueMask;
        }
        static uint32_t AlphaMask() {
            return DefaultAlphaMask;
        }
        static uint32_t ColorKey() {
            return TGX::Transparent8;
        }
        static void Load(SDL_RWops *src, int64_t size, const ImageHeader &, Surface &surface) {
            TGX::DecodeTGX(src, size, surface);
        }
    };

    struct TGX16
    {
        static int Width(const ImageHeader &header) {
            return header.width;
        }    
        static int Height(const ImageHeader &header) {
            return header.height;
        }    
        static int Depth() {
            return 16;
        }    
        static uint32_t RedMask() {
            return TGX::RedMask16;
        }    
        static uint32_t GreenMask() {
            return TGX::GreenMask16;
        }    
        static uint32_t BlueMask() {
            return TGX::BlueMask16;
        }    
        static uint32_t AlphaMask() {
            return TGX::AlphaMask16;
        }
        static uint32_t ColorKey() {
            return TGX::Transparent16;
        }    
        static void Load(SDL_RWops *src, int64_t size, const ImageHeader &, Surface &surface) {
            TGX::DecodeTGX(src, size, surface);
        }
    };

    struct TileObject
    {
        static int Width(const ImageHeader &) {
            return TGX::TileWidth;
        }    
        static int Height(const ImageHeader &header) {
            return TGX::TileHeight + header.tileY;
        }    
        static int Depth() {
            return 16;
        }    
        static uint32_t RedMask() {
            return TGX::RedMask16;
        }    
        static uint32_t GreenMask() {
            return TGX::GreenMask16;
        }    
        static uint32_t BlueMask() {
            return TGX::BlueMask16;
        }    
        static uint32_t AlphaMask() {
            return TGX::AlphaMask16;
        }    
        static uint32_t ColorKey() {
            return TGX::Transparent16;
        }
        static void Load(SDL_RWops *src, int64_t size, const ImageHeader &header, Surface &surface) {
            SDL_Rect tilerect = MakeRect(0, header.tileY, Width(header), TGX::TileHeight);
            SurfaceROI tile(surface, &tilerect);
            TGX::DecodeTile(src, TGX::TileBytes, tile);
        
            SDL_Rect boxrect = MakeRect(header.hOffset, 0, header.boxWidth, Height(header));
            SurfaceROI box(surface, &boxrect);
            TGX::DecodeTGX(src, size - TGX::TileBytes, box);
        }
    };

    struct Bitmap
    {
        static int Width(const ImageHeader &header) {
            return header.width;
        }    
        static int Height(const ImageHeader &header) {
            // Nobody knows why
            return header.height - 7;
        }    
        static int Depth() {
            return 16;
        }    
        static uint32_t RedMask() {
            return TGX::RedMask16;
        }    
        static uint32_t GreenMask() {
            return TGX::GreenMask16;
        }    
        static uint32_t BlueMask() {
            return TGX::BlueMask16;
        }    
        static uint32_t AlphaMask() {
            return TGX::AlphaMask16;
        }    
        static uint32_t ColorKey() {
            return TGX::Transparent16;
        }    
        static void Load(SDL_RWops *src, int64_t size, const ImageHeader &, Surface &surface) {
            TGX::DecodeUncompressed(src, size, surface);
        }
    };

}

namespace GM
{

    Collection::Collection(SDL_RWops *src)
    {
        if(src == NULL) {
            Fail(__FUNCTION__, "NULL src");
        }
    
        header = ReadHeader(src);
        if(ReadableBytes(src) < header.dataSize) {
            Fail(__FUNCTION__, "Premate EOF");
        }
        
        palettes.resize(CollectionPaletteCount);
        for(Palette &palette : palettes)
            palette = ReadPalette(src);

        uint32_t count = header.imageCount;
        offsets.resize(count);
        ReadInt32ArrayLE(src, offsets.data(), offsets.size());
        
        sizes.resize(count);
        ReadInt32ArrayLE(src, sizes.data(), sizes.size());

        headers.resize(count);
        for(ImageHeader &hdr : headers)
            hdr = ReadImageHeader(src);
    }

    Encoding Collection::encoding() const
    {
        return GetEncoding(header.dataClass);
    }

    size_t Collection::size() const
    {
        return header.imageCount;
    }

    Surface LoadEntry(SDL_RWops *src, const Collection &gm1, size_t index)
    {
        switch(gm1.encoding()) {
        case Encoding::Font:
            /* fallthrough */
        case Encoding::TGX16:
            return LoadEntryImpl<TGX16>(src, gm1, index);
        case Encoding::TGX8:
            return LoadEntryImpl<TGX16>(src, gm1, index);
        case Encoding::TileObject:
            return LoadEntryImpl<TGX16>(src, gm1, index);
        case Encoding::Bitmap:
            return LoadEntryImpl<TGX16>(src, gm1, index);
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }

    Surface LoadAtlas(SDL_RWops *src, const Collection &gm1)
    {
        switch(gm1.encoding()) {
        case Encoding::Font:
            /* fallthrough */
        case Encoding::TGX16:
            return LoadAtlasImpl<TGX16>(src, gm1);
        case Encoding::TGX8:
            return LoadAtlasImpl<TGX8>(src, gm1);
        case Encoding::Bitmap:
            return LoadAtlasImpl<Bitmap>(src, gm1);
        case Encoding::TileObject:
            return LoadAtlasImpl<TileObject>(src, gm1);
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }

    SDL_Rect PartitionAtlas(const Collection &gm1, std::vector<SDL_Rect> &rects)
    {
        switch(gm1.encoding()) {
        case Encoding::Font:
            /* fallthrough */
        case Encoding::TGX16:
            return PartitionAtlasImpl<TGX16>(gm1, rects);
        case Encoding::TGX8:
            return PartitionAtlasImpl<TGX8>(gm1, rects);
        case Encoding::Bitmap:
            return PartitionAtlasImpl<Bitmap>(gm1, rects);
        case Encoding::TileObject:
            return PartitionAtlasImpl<TileObject>(gm1, rects);
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }

    int LoadEntries(SDL_RWops *src, const Collection &scheme, std::vector<Surface> &atlas)
    {
        switch(scheme.encoding()) {
        case Encoding::Font:
            /* fallthrough */
        case Encoding::TGX16:
            return LoadEntriesImpl<TGX16>(src, scheme, atlas);
        case Encoding::TileObject:
            return LoadEntriesImpl<TileObject>(src, scheme, atlas);
        case Encoding::Bitmap:
            return LoadEntriesImpl<Bitmap>(src, scheme, atlas);
        case Encoding::TGX8:
            return LoadEntriesImpl<TGX8>(src, scheme, atlas);
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }

    void PrintImageHeader(std::ostream &out, const ImageHeader &header)
    {
        using namespace std;
        out << "Width: "            << static_cast<int>(header.width) << endl
            << "Height: "           << static_cast<int>(header.height) << endl
            << "PosX: "             << static_cast<int>(header.posX) << endl
            << "PosY: "             << static_cast<int>(header.posY) << endl
            << "Group: "            << static_cast<int>(header.group) << endl
            << "GroupSize: "        << static_cast<int>(header.groupSize) << endl
            << "TileY: "            << static_cast<int>(header.tileY) << endl
            << "TileOrient: "       << static_cast<int>(header.tileOrient) << endl
            << "Horiz Offset: "     << static_cast<int>(header.hOffset) << endl
            << "Box Width: "        << static_cast<int>(header.boxWidth) << endl
            << "Flags: "            << static_cast<int>(header.flags) << endl;
    }

    void PrintHeader(std::ostream &out, const Header &header)
    {
        using namespace std;
        out << "u1: "               << header.u1 << endl
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

    void PrintPalette(std::ostream &out, const Palette &palette)
    {
        int column = 0;

        out << std::hex;
        for(auto color : palette) {
            column++;
            
            if(column % 16 == 0)
                out << std::endl;
            
            out << color << ' ';
        }

        out << std::endl;
    }

    void PrintCollection(std::ostream &out, const Collection &gm1)
    {
        using namespace std;
    
        PrintHeader(out, gm1.header);
        for(size_t index = 0; index < gm1.palettes.size(); ++index) {
            out << "Palette " << index << endl;
            PrintPalette(out, gm1.palettes[index]);
        }
    
        for(size_t i = 0; i < gm1.offsets.size(); ++i)
            out << "Offset " << i << ": " << gm1.offsets[i] << endl;
    
        for(size_t i = 0; i < gm1.sizes.size(); ++i)
            out << "Size " << i << ": " << gm1.sizes[i] << endl;
    
        for(size_t i = 0; i < gm1.headers.size(); ++i) {
            out << "Image Header " << i << endl;
            PrintImageHeader(out, gm1.headers[i]);
        }
    
        int64_t origin =
            // unaligned sizeof(GM::Header)
            CollectionHeaderBytes
            // unaligned sizeof(GM::ImageHeader) * n
            + CollectionEntryHeaderBytes * gm1.size()
            // unaligned sizeof(GM::Palette) * m
            + CollectionPaletteColors * CollectionPaletteCount * sizeof(uint16_t)
            + sizeof(uint32_t) * gm1.size()
            + sizeof(uint32_t) * gm1.size();
        out << "Data entry point at " << dec << origin << endl;
    }

} // namespace GM

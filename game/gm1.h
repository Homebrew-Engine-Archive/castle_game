#ifndef GM1_H_
#define GM1_H_

#include <iosfwd>
#include <array>
#include <vector>
#include <stdexcept>
#include <SDL.h>

class Surface;

namespace GM1
{

    typedef uint16_t PaletteEntry;
    
    const size_t CollectionPaletteCount = 10;
    const size_t CollectionPaletteColors = 256;
    const size_t CollectionPaletteBytes = CollectionPaletteColors * sizeof(PaletteEntry);
    const size_t CollectionEntryHeaderBytes = 16;
    const size_t CollectionHeaderBytes = 88;
    const size_t PaletteTransparentIndex = 0;
    const size_t PaletteShadowIndex = 1;

    /**
     * \brief Is included in every collection only once.
     *
     * Header has size exactly 88 bytes.
     *
     * `imageCount' is a just number of entries in the archive.
     *
     * Each entry shall be decoded by GM1::Encoding which
     * is determined by `dataClass' field (see GetEncoding function).
     *
     * \note There are many fields have undefined purpose. Each of them prefix with 'u'.
     */
    struct Header
    {
        uint32_t u1;
        uint32_t u2;
        uint32_t u3;
        uint32_t imageCount;
        uint32_t u4;
        uint32_t dataClass;
        uint32_t u5;
        uint32_t u6;
        uint32_t sizeCategory;
        uint32_t u7;
        uint32_t u8;
        uint32_t u9;
        uint32_t width;
        uint32_t height;
        uint32_t u10;
        uint32_t u11;
        uint32_t u12;
        uint32_t u13;
        uint32_t anchorX;
        uint32_t anchorY;
        uint32_t dataSize;
        uint32_t u14;
    };

    /**
     * \brief Every image in the collection describes by this.
     *
     * This header should be read from the source file before
     * read original image.
     *
     * \note If collection is TileObject encoded, then `tileY' represents
     * an offset from the top pixel of box to the bottom.
     * 
     * \note If collection is Font encoded, then `tileY' represents
     * a y-bearing font metric.
     * see http://freetype.org/freetype2/docs/glyphs/glyphs-3.html
     *
     */
    struct EntryHeader
    {
        uint16_t width;
        uint16_t height;
        uint16_t posX;
        uint16_t posY;
        uint8_t group;
        uint8_t groupSize;
        int16_t tileY;
        uint8_t tileOrient;                              // TileAlignment
        uint8_t hOffset;
        uint8_t boxWidth;
        uint8_t flags;
    };

    enum class Encoding : int
    {
        TGX16,
        TGX8,
        Font,
        TileObject,
        Bitmap,
        Unknown
    };

    // TODO Look how this thing really works (if it does).
    // enum class TileAlignment : uint8_t {
    //     Left,
    //     Right,
    //     Center,
    //     None
    // };

    typedef std::array<PaletteEntry, CollectionPaletteColors> Palette;

    enum class PaletteSet : size_t
    {
        Unknown0,
        Blue,
        Red,
        Orange,
        Yellow,
        Purple,
        Black,
        Cyan,
        Green,
        Unknown1
    };

    struct Collection
    {
        explicit Collection(SDL_RWops *src);
        
        Header header;
        std::vector<Palette> palettes;
        std::vector<uint32_t> offsets, sizes;
        std::vector<EntryHeader> headers;        
        Encoding encoding() const;
        size_t size() const;
    };

    GM1::Encoding GetEncoding(uint32_t dataClass);
    
    /**
     * \brief Calculates size of "static" part of the archive by its header.
     */
    size_t GetPreambleSize(GM1::Header const&);
    
    void PrintEntryHeader(std::ostream &out, const EntryHeader &header);
    void PrintHeader(std::ostream &out, const Header &gm1);
    void PrintPalette(std::ostream &out, const Palette &palette);
    void PrintCollection(std::ostream &out, const Collection &collection);

    int LoadEntries(SDL_RWops *src, const Collection &gm1, std::vector<Surface> &atlas);
    Surface LoadAtlas(SDL_RWops *src, const Collection &gm1);
    SDL_Rect PartitionAtlas(const Collection &gm1, std::vector<SDL_Rect> &);
    Surface LoadEntry(SDL_RWops *src, const Collection &gm1, size_t index);

} // namespace GM1

#endif

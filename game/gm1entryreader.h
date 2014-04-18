#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include <iosfwd>
#include <memory>
#include <game/tgx.h>
#include <game/surface.h>

namespace GM1
{
    class GM1Reader;
    class EntryHeader;
    class Header;
    enum class Encoding;
}

namespace GM1
{
    const size_t TileBytes = 512;
    const size_t TileWidth = 30;
    const size_t TileHeight = 16;

    class EntryRenderer
    {
    public:
        Surface Render(std::istream &in, size_t numBytes, GM1::EntryHeader const&);
    };
    
    class GM1EntryReader
    {
    protected:
        Surface CreateSurface(const GM1::EntryHeader &header) const;
        virtual void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const = 0;
        
    public:
        constexpr int Width(GM1::EntryHeader const&);
        constexpr virtual int Height(GM1::EntryHeader const&);
        constexpr virtual int Depth();
        constexpr virtual uint32_t RedMask();
        constexpr virtual uint32_t GreenMask();
        constexpr virtual uint32_t BlueMask();
        constexpr virtual uint32_t AlphaMask();
        constexpr virtual uint32_t ColorKey();

        virtual bool Palettized() const;
        
        Surface Load(const GM1::GM1Reader &reader, size_t index) const;
        
    };
    
    std::unique_ptr<GM1EntryReader> CreateEntryReader(GM1::Encoding const&);
    
}

#endif

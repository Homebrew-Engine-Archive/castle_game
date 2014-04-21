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
        
    class GM1EntryReader
    {
        SDL_Color mTransparentColor;
        
    protected:
        virtual Surface CreateCompatibleSurface(const GM1::EntryHeader &header) const;
        virtual void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const = 0;
        
    public:
        GM1EntryReader();

        void Transparent(SDL_Color color);
        SDL_Color Transparent() const;
        
        virtual int Width(GM1::EntryHeader const&) const;
        virtual int Height(GM1::EntryHeader const&) const;
        virtual uint32_t GetColorKey() const;
        virtual int CompatiblePixelFormat() const;
        virtual bool Palettized() const;

        Surface Load(const GM1::GM1Reader &reader, size_t index) const;

        typedef std::unique_ptr<GM1EntryReader> Ptr;
    };
    
    GM1EntryReader::Ptr CreateEntryReader(GM1::Encoding const&);
    
}

#endif

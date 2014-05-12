#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include <iosfwd>
#include <memory>

#include <game/tgx.h>
#include <game/surface.h>
#include <game/color.h>
#include <game/point.h>

namespace GM1
{
    class GM1Reader;
    class EntryHeader;
    class Header;
    enum class Encoding;
}

namespace GM1
{        
    class GM1EntryReader
    {
        Color mTransparentColor;
        
    protected:
        virtual Surface CreateCompatibleSurface(const GM1::EntryHeader &header) const;
        virtual void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const = 0;
        
    public:
        GM1EntryReader();

        void Transparent(Color color);
        Color Transparent() const;
        
        virtual int Width(GM1::EntryHeader const&) const;
        virtual int Height(GM1::EntryHeader const&) const;
        virtual uint32_t GetColorKey() const;
        virtual int CompatiblePixelFormat() const;
        virtual bool Palettized() const;
        virtual Point ImageCenter(GM1::Header const&, GM1::EntryHeader const&) const;

        Surface Load(const GM1::GM1Reader &reader, size_t index) const;

        typedef std::unique_ptr<GM1EntryReader> Ptr;
    };
    
    GM1EntryReader::Ptr CreateEntryReader(GM1::Encoding const&);
}

#endif

#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include <iosfwd>
#include <memory>

#include <game/color.h>

namespace GM1
{
    class GM1Reader;
    class EntryHeader;
    class Header;
    enum class Encoding;
}

class Surface;

namespace GM1
{
    class GM1EntryReader
    {
        Color mTransparentColor;

    private:
        Surface CreateCompatibleSurface(const GM1::EntryHeader &header) const;
        uint32_t GetColorKey(uint32_t format) const;
        
    protected:
        virtual void ReadSurface(std::istream &in, size_t numBytes, const GM1::EntryHeader &header, Surface &surface) const = 0;

        virtual int Width(GM1::EntryHeader const&) const;
        virtual int Height(GM1::EntryHeader const&) const;

        /**
           Convert from source to target if necessary.
           \todo does it violate LSP?
         **/
        virtual uint32_t SourcePixelFormat() const;
        virtual uint32_t TargetPixelFormat() const;
        
    public:
        GM1EntryReader();
        virtual ~GM1EntryReader() = default;
        
        void Transparent(Color color);
        const Color Transparent() const;

        const Surface Load(const GM1::GM1Reader &reader, size_t index) const;

        typedef std::unique_ptr<GM1EntryReader> Ptr;
    };
    
    GM1EntryReader::Ptr CreateEntryReader(GM1::Encoding const&);
}

#endif

#ifndef GM1ENTRYREADER_H_
#define GM1ENTRYREADER_H_

#include <iosfwd>
#include <memory>

#include <core/color.h>

namespace gm1
{
    class EntryHeader;
    class Header;
    enum class ArchiveType;
}

namespace castle
{
    class Image;
}

namespace gm1
{
    class GM1EntryReader
    {
        core::Color mTransparentColor;

    private:
        castle::Image CreateCompatibleImage(const gm1::EntryHeader &header) const;
        uint32_t GetColorKey(uint32_t format) const;
        
    protected:
        virtual void ReadImage(std::istream &in, size_t numBytes, const gm1::EntryHeader &header, castle::Image &surface) const = 0;
        virtual int Width(const gm1::EntryHeader &header) const;
        virtual int Height(const gm1::EntryHeader &header) const;
        virtual uint32_t SourcePixelFormat() const;
        
    public:
        GM1EntryReader();
        virtual ~GM1EntryReader() = default;
        
        void Transparent(core::Color color);
        const core::Color Transparent() const;
        const castle::Image Load(const gm1::EntryHeader &header, const char *data, size_t bytesCount) const;

        typedef std::unique_ptr<GM1EntryReader> Ptr;
    };
    
    GM1EntryReader::Ptr CreateEntryReader(const gm1::ArchiveType &type);
}

#endif  // GM1ENTRYHEADER_H_

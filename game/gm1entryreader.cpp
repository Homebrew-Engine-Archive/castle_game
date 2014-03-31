#include "gm1entryreader.h"

#include <sstream>
#include <boost/current_function.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <SDL.h>

#include "geometry.h"
#include "make_unique.h"
#include "gm1reader.h"
#include "gm1.h"
#include "tgx.h"
#include "surface.h"

namespace
{

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
    
    GM1::Encoding GetEncoding(uint32_t dataClass)
    {
        switch(dataClass) {
        case 1: return GM1::Encoding::TGX16;
        case 2: return GM1::Encoding::TGX8;
        case 3: return GM1::Encoding::TileObject;
        case 4: return GM1::Encoding::Font;
        case 5: return GM1::Encoding::Bitmap;
        case 6: return GM1::Encoding::TGX16;
        case 7: return GM1::Encoding::Bitmap;
        default: return GM1::Encoding::Unknown;
        }
    }
    
    class TGX8 : public GM1::GM1EntryReader
    {
    public:
        int Depth() const {
            return 8;
        }
        
        uint32_t RedMask() const {
            return DefaultRedMask;
        }
        
        uint32_t GreenMask() const {
            return DefaultGreenMask;
        }
        
        uint32_t BlueMask() const {
            return DefaultBlueMask;
        }
        
        uint32_t AlphaMask() const {
            return DefaultAlphaMask;
        }
        
        uint32_t ColorKey() const {
            return TGX::Transparent8;
        }

    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    class TGX16 : public GM1::GM1EntryReader
    {
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    class TileObject : public GM1::GM1EntryReader
    {
    public:
        int Width(GM1::EntryHeader const&) const {
            return TGX::TileWidth;
        }
        
        int Height(const GM1::EntryHeader &header) const {
            return TGX::TileHeight + header.tileY;
        }
        
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    class Bitmap : public GM1::GM1EntryReader
    {
    public:
        int Height(const GM1::EntryHeader &header) const {
            // Nobody knows why
            return header.height - 7;
        }
        
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    void TGX8::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        TGX::DecodeTGX(in, numBytes, surface);
    }

    void TGX16::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        TGX::DecodeTGX(in, numBytes, surface);
    }

    void Bitmap::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        TGX::DecodeBitmap(in, numBytes, surface);
    }

    void TileObject::ReadSurface(std::istream &in, size_t numBytes, const GM1::EntryHeader &header, Surface &surface) const
    {
        SDL_Rect tilerect = MakeRect(0, header.tileY, Width(header), TGX::TileHeight);
        SurfaceROI tile(surface, &tilerect);
        TGX::DecodeTile(in, TGX::TileBytes, tile);

        SDL_Rect boxrect = MakeRect(header.hOffset, 0, header.boxWidth, Height(header));
        SurfaceROI box(surface, &boxrect);
        TGX::DecodeTGX(in, numBytes - TGX::TileBytes, box);
    }
    
}

namespace GM1
{

    Surface GM1EntryReader::CreateSurface(const GM1::EntryHeader &header) const
    {
        int width = Width(header);
        int height = Height(header);
        
        Surface surface = SDL_CreateRGBSurface(
            0, width, height, Depth(),
            RedMask(),
            GreenMask(),
            BlueMask(),
            AlphaMask());
        
        if(surface.Null()) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
        
        SDL_SetColorKey(surface, SDL_TRUE, ColorKey());
        SDL_FillRect(surface, NULL, ColorKey());

        return surface;
    }

    Surface GM1EntryReader::Load(GM1::GM1Reader &reader, size_t index) const
    {
        GM1::EntryHeader header = reader.EntryHeader(index);
        
        Surface surface = CreateSurface(header);

        const char *data = reader.EntryData(index);
        size_t size = reader.EntrySize(index);
        
        boost::iostreams::stream<boost::iostreams::array_source> in(data, size);
        ReadSurface(in, size, header, surface);
        
        return surface;
    }
    
    int GM1EntryReader::Width(const GM1::EntryHeader &header) const
    {
        return header.width;
    }

    int GM1EntryReader::Height(const GM1::EntryHeader &header) const
    {
        return header.height;
    }

    int GM1EntryReader::Depth() const
    {
        return 16;
    }

    uint32_t GM1EntryReader::RedMask() const
    {
        return TGX::RedMask16;
    }

    uint32_t GM1EntryReader::GreenMask() const
    {
        return TGX::GreenMask16;
    }

    uint32_t GM1EntryReader::BlueMask() const
    {
        return TGX::BlueMask16;
    }

    uint32_t GM1EntryReader::AlphaMask() const
    {
        return TGX::AlphaMask16;
    }
    
    uint32_t GM1EntryReader::ColorKey() const
    {
        return TGX::Transparent16;
    }
    
    std::unique_ptr<GM1EntryReader> CreateEntryReader(const GM1::GM1Reader &reader)
    {
        switch(GetEncoding(reader.Header().dataClass)) {
        case GM1::Encoding::Font:
            /* fallthrough */
        case GM1::Encoding::TGX16:
            return std::unique_ptr<GM1EntryReader>(new TGX16);
                
        case GM1::Encoding::Bitmap:
            return std::unique_ptr<GM1EntryReader>(new Bitmap);
        
        case GM1::Encoding::TGX8:
            return std::unique_ptr<GM1EntryReader>(new TGX8);
            
        case GM1::Encoding::TileObject:
            return std::unique_ptr<GM1EntryReader>(new TileObject);
            
        case GM1::Encoding::Unknown:
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }
    
}

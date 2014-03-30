#include "gm1entryreader.h"

#include <sstream>
#include <boost/current_function.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <SDL.h>

#include "make_unique.h"
#include "gm1reader.h"
#include "gm1.h"
#include "tgx.h"

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

    template<class EntryClass>
    Surface CreateCompatibleSurface(const GM1::EntryHeader &header)
    {
        EntryClass format;

        int width = format.Width(header);
        int height = format.Height(header);
        
        Surface surface = SDL_CreateRGBSurface(
            0, width, height, format.Depth(),
            format.RedMask(),
            format.GreenMask(),
            format.BlueMask(),
            format.AlphaMask());
        
        if(surface.Null()) {
            Fail(__FILE__, __LINE__, SDL_GetError());
        }
        
        SDL_SetColorKey(surface, SDL_TRUE, format.ColorKey());
        SDL_FillRect(surface, NULL, format.ColorKey());

        return surface;
    }
    
    class TGX8
    {
    public:
        int Width(const EntryHeader &header) const {
            return header.width;
        }
        
        int Height(const EntryHeader &header) const {
            return header.height;
        }
        
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
        
        void Read(GM1::GM1Reader &reader, size_t index, Surface &surface) const {
            
        }
    };

    class TGX16
    {
    public:
        int Width(const GM1::EntryHeader &header) const {
            return header.width;
        }
        
        int Height(const GM1::EntryHeader &header) const {
            return header.height;
        }
        
        int Depth() const {
            return 16;
        }
        
        uint32_t RedMask() const {
            return TGX::RedMask16;
        }
        
        uint32_t GreenMask() const {
            return TGX::GreenMask16;
        }
        
        uint32_t BlueMask() const {
            return TGX::BlueMask16;
        }
        
        uint32_t AlphaMask() const {
            return TGX::AlphaMask16;
        }
        
        uint32_t ColorKey() const {
            return TGX::Transparent16;
        }
        
        void Read(GM1::GM1Reader &reader, size_t index, Surface &surface) const {
            
        }
        
    };

    class TileObject
    {
    public:
        int Width(GM1::EntryHeader const&) const {
            return TGX::TileWidth;
        }
        
        int Height(const GM1::EntryHeader &header) const {
            return TGX::TileHeight + header.tileY;
        }
        
        int Depth() const {
            return 16;
        }
        
        uint32_t RedMask() const {
            return TGX::RedMask16;
        }
        
        uint32_t GreenMask() const {
            return TGX::GreenMask16;
        }
        
        uint32_t BlueMask() const {
            return TGX::BlueMask16;
        }
        
        uint32_t AlphaMask() const {
            return TGX::AlphaMask16;
        }
        
        uint32_t ColorKey() const {
            return TGX::Transparent16;
        }
        
        void Read(GM1::GM1Reader &reader, size_t index, Surface &surface) const {
            
        }
    };

    class Bitmap
    {
    public:
        int Width(const GM1::EntryHeader &header) const {
            return header.width;
        }
        
        int Height(const GM1::EntryHeader &header) const {
            // Nobody knows why
            return header.height - 7;
        }
        
        int Depth() const {
            return 16;
        }
        
        uint32_t RedMask() const {
            return TGX::RedMask16;
        }
        
        uint32_t GreenMask() const {
            return TGX::GreenMask16;
        }
        
        uint32_t BlueMask() const {
            return TGX::BlueMask16;
        }
        
        uint32_t AlphaMask() const {
            return TGX::AlphaMask16;
        }
        
        uint32_t ColorKey() const {
            return TGX::Transparent16;
        }
        
        void Read(GM1::GM1Reader &reader, size_t index, Surface &surface) const {
            
        }
    };
    
}

namespace GM1
{
    
    template<class EntryClass>
    Surface ReadSurface(GM1::GM1Reader &reader, size_t index)
    {
        Surface surface = CreateCompatibleSurface<EntryClass>(reader.EntryHeaders(index));
        EntryClass::Read(reader, index, surface);
        return surface;
    }
    
    Surface GM1EntryReader::ReadSurface(size_t index) const
    {


    }
}

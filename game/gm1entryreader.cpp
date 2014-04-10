#include "gm1entryreader.h"

#include <sstream>
#include <boost/current_function.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <SDL.h>

#include <game/geometry.h>
#include <game/make_unique.h>
#include <game/gm1reader.h>
#include <game/gm1.h>
#include <game/tgx.h>
#include <game/surface.h>

namespace
{

    const size_t TileBytes = 512;
    const size_t TileWidth = 30;
    const size_t TileHeight = 16;
    
    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }

    /**
     * \brief Reader for animation sprites.
     *
     * 8-bit images;
     * TGX-compressed;
     */
    class TGX8 : public GM1::GM1EntryReader
    {
    public:
        constexpr int Depth() {
            return 8;
        }
        
        constexpr uint32_t RedMask() {
            return DefaultRedMask;
        }
        
        constexpr uint32_t GreenMask() {
            return DefaultGreenMask;
        }
        
        constexpr uint32_t BlueMask() {
            return DefaultBlueMask;
        }
        
        constexpr uint32_t AlphaMask() {
            return DefaultAlphaMask;
        }
        
        constexpr uint32_t ColorKey() {
            return TGX::Transparent8;
        }

    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    /**
     * \brief Read for static textures.
     *
     * All fonts are such encoded.
     *
     * 16-bit images;
     * TGX-compressed;
     */
    class TGX16 : public GM1::GM1EntryReader
    {
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    class FontReader : public GM1::GM1EntryReader
    {
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };
    
    /**
     * \brief Reader for tile textures.
     *
     * Every tile is composed of 30x16 tile rhombus texture and
     * static tgx16 decoration.
     *
     * Tiles are decoded by TGX::DecodeTile
     *
     */
    class TileObject : public GM1::GM1EntryReader
    {
    public:
        constexpr int Width(GM1::EntryHeader const&) {
            return TileWidth;
        }
        
        constexpr int Height(const GM1::EntryHeader &header) {
            return TileHeight + header.tileY;
        }
        
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };

    /**
     * \tile Uncompressed images. 
     */
    class Bitmap : public GM1::GM1EntryReader
    {
    public:
        constexpr int Height(const GM1::EntryHeader &header) {
            // Nobody knows why
            return header.height - 7;
        }
        
    protected:
        void ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const;
    };
    
    void TGX8::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        TGX::DecodeSurface(in, numBytes, surface);
    }

    void TGX16::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        TGX::DecodeSurface(in, numBytes, surface);
    }

    void FontReader::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        TGX::DecodeSurface(in, numBytes, surface);

        // Originally I found that just color-keying of an image
        // doesn't work properly. After skipping all fully-transparent
        // pixels of the image some opaque magenta "halo" still remains.
        //
        // In the way to solve it i'd just swapped green channel with alpha
        // and go clean my hands.
        // 
        // TODO is there a better way to do so?
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, NoFlags);
        if(surface.Null()) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }

        // Here we just ignore original color information. What we are really
        // interested in is green channel
        auto swap_green_alpha = [](uint8_t, uint8_t g, uint8_t, uint8_t) {
            return SDL_Color { 255, 255, 255, g };
        };
    
        MapSurface(surface, swap_green_alpha);
    }

    void ReadBitmap(std::istream &in, size_t numBytes, Surface &surface)
    {
        const SurfaceLocker lock(surface);

        const int bytesPerPixel = surface->format->BytesPerPixel;
        const size_t rowBytes = surface->w * bytesPerPixel;
        char *dst = GetPixels(surface);
    
        while(numBytes >= rowBytes) {
            in.read(dst, rowBytes);
            dst += surface->pitch;
            numBytes -= rowBytes;
        }
    }
    
    void Bitmap::ReadSurface(std::istream &in, size_t numBytes, GM1::EntryHeader const&, Surface &surface) const
    {
        ReadBitmap(in, numBytes, surface);
    }
    
    // Width of rhombus rows in pixels.
    // \todo should it be placed in separate header file?
    inline int GetTilePixelsPerRow(int row)
    {
        static const int PerRow[] = {2, 6, 10, 14, 18, 22, 26, 30, 30, 26, 22, 18, 14, 10, 6, 2};
        return PerRow[row];
    }

    void ReadTile(std::istream &in, size_t numBytes, Surface &surface)
    {
        if(numBytes < TileBytes) {
            Fail(BOOST_CURRENT_FUNCTION, "Inconsistent tile size");
        }

        const SurfaceLocker lock(surface);

        const int pitch = surface->pitch;
        const int height = TileHeight;
        const int width = TileWidth;
        const int bytesPerPixel = surface->format->BytesPerPixel;
        char *dst = GetPixels(surface);
    
        for(int y = 0; y < height; ++y) {
            const int length = GetTilePixelsPerRow(y);
            const int offset = (width - length) / 2;
            in.read(dst + offset * bytesPerPixel, length * bytesPerPixel);
            dst += pitch;
        }
    }
    
    void TileObject::ReadSurface(std::istream &in, size_t numBytes, const GM1::EntryHeader &header, Surface &surface) const
    {
        SDL_Rect tilerect = MakeRect(0, header.tileY, Width(header), TileHeight);
        SurfaceROI tile(surface, &tilerect);
        ReadTile(in, TileBytes, tile);

        SDL_Rect boxrect = MakeRect(header.hOffset, 0, header.boxWidth, Height(header));
        SurfaceROI box(surface, &boxrect);
        TGX::DecodeSurface(in, numBytes - TileBytes, box);
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

    Surface GM1EntryReader::Load(const GM1::GM1Reader &reader, size_t index) const
    {
        const GM1::EntryHeader &header = reader.EntryHeader(index);

        Surface surface = CreateSurface(header);

        const char *data = reader.EntryData(index);
        size_t size = reader.EntrySize(index);
        
        boost::iostreams::stream<boost::iostreams::array_source> in(data, size);
        ReadSurface(in, size, header, surface);
        
        return surface;
    }
    
    constexpr int GM1EntryReader::Width(const GM1::EntryHeader &header)
    {
        return header.width;
    }

    constexpr int GM1EntryReader::Height(const GM1::EntryHeader &header)
    {
        return header.height;
    }

    constexpr int GM1EntryReader::Depth()
    {
        return 16;
    }

    constexpr uint32_t GM1EntryReader::RedMask()
    {
        return TGX::RedMask16;
    }

    constexpr uint32_t GM1EntryReader::GreenMask()
    {
        return TGX::GreenMask16;
    }

    constexpr uint32_t GM1EntryReader::BlueMask()
    {
        return TGX::BlueMask16;
    }

    constexpr uint32_t GM1EntryReader::AlphaMask()
    {
        return TGX::AlphaMask16;
    }
    
    constexpr uint32_t GM1EntryReader::ColorKey()
    {
        return TGX::Transparent16;
    }

    bool GM1EntryReader::Palettized() const
    {
        return Depth() == 8;
    }
    
    std::unique_ptr<GM1EntryReader> CreateEntryReader(const GM1::Encoding &encoding)
    {
        switch(encoding) {
        case GM1::Encoding::Font:
            return std::unique_ptr<GM1EntryReader>(new FontReader);
            
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

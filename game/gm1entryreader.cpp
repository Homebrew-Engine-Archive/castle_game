#include "gm1entryreader.h"

#include <SDL.h>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <game/rect.h>
#include <game/sdl_utils.h>
#include <game/gm1reader.h>
#include <game/gm1palette.h>
#include <game/gm1.h>
#include <game/tgx.h>
#include <game/surface.h>

namespace
{    
    /**
     * \brief Reader for animation sprites.
     *
     * 8-bit images;
     * TGX-compressed;
     */
    class TGX8 : public GM1::GM1EntryReader
    {
    public:
        int CompatiblePixelFormat() const {
            return SDL_PIXELFORMAT_INDEX8;
        }

        Point ImageCenter(const GM1::Header &header, GM1::EntryHeader const&) const {
            return Point(header.anchorX, header.anchorY);
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
        int Width(GM1::EntryHeader const&) const {
            return GM1::TileSpriteWidth;
        }
        
        int Height(const GM1::EntryHeader &header) const {
            return GM1::TileSpriteHeight + header.tileY;
        }

        Point ImageCenter(const GM1::Header &header, const GM1::EntryHeader &entry) const {
            return Point(GM1::TileSpriteWidth / 2, GM1::TileSpriteHeight / 2) + Point(0, entry.tileY);
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
        int Height(const GM1::EntryHeader &header) const {
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

        // In the way to solve it i'd just swapped green channel with alpha
        // and go clean my hands.

        // TODO is there a better way to do so?
        // Surface tmp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
        // if(!tmp) {
        //     throw sdl_error();
        // }

        // uint32_t colorkey = GetColorKey();
        // if(SDL_SetColorKey(tmp, SDL_TRUE, colorkey) < 0) {
        //     throw sdl_error();
        // }

        // Here we just ignore original color information. What we are really
        // interested in is green channel
        // auto swap_green_alpha = [](const Color &color) {
        //     return Color(color.r, 255, color.b, color.g);
        // };
    
        // TransformSurface(tmp, swap_green_alpha);

        // surface = tmp;
    }

    void ReadBitmap(std::istream &in, size_t numBytes, Surface &surface)
    {
        const SurfaceLocker lock(surface);

        const size_t rowBytes = surface->w * surface->format->BytesPerPixel;
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
    constexpr int PerRow[] = {2, 6, 10, 14, 18, 22, 26, 30, 30, 26, 22, 18, 14, 10, 6, 2};
    
    constexpr int GetTilePixelsPerRow(int row)
    {
        return PerRow[row];
    }

    void ReadTile(std::istream &in, Surface &surface)
    {
        const SurfaceLocker lock(surface);

        const int pitch = surface->pitch;
        const int height = GM1::TileSpriteHeight;
        const int width = GM1::TileSpriteWidth;
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
        Rect tilerect(0, header.tileY, Width(header), GM1::TileSpriteHeight);
        SurfaceView tile(surface, tilerect);
        ReadTile(in, tile.View());
        
        Rect boxrect(header.hOffset, 0, header.boxWidth, Height(header));
        SurfaceView box(surface, boxrect);
        TGX::DecodeSurface(in, numBytes - GM1::TileBytes, box.View());
    }
}

namespace GM1
{
    GM1EntryReader::GM1EntryReader()
        : mTransparentColor(240, 0, 255)
    {
    }
    
    Surface GM1EntryReader::CreateCompatibleSurface(const GM1::EntryHeader &header) const
    {
        const int width = Width(header);
        const int height = Height(header);
        
        Surface surface = CreateSurface(width, height, CompatiblePixelFormat());
        if(!surface) {
            throw sdl_error();
        }
        
        uint32_t colorkey = GetColorKey();
        if(SDL_SetColorKey(surface, SDL_RLEACCEL, colorkey) < 0) {
            throw sdl_error();
        }
        
        if(SDL_FillRect(surface, NULL, colorkey) < 0) {
            throw sdl_error();
        }
                
        return surface;
    }

    Surface GM1EntryReader::Load(const GM1::GM1Reader &reader, size_t index) const
    {
        const GM1::EntryHeader &header = reader.EntryHeader(index);

        Surface surface = CreateCompatibleSurface(header);

        const char *data = reader.EntryData(index);
        const size_t size = reader.EntrySize(index);
        
        boost::iostreams::stream<boost::iostreams::array_source> in(data, size);
        ReadSurface(in, size, header, surface);

        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
        if(!surface) {
            throw sdl_error();
        }
        
        if(SDL_SetSurfaceRLE(surface, SDL_TRUE) < 0) {
            throw sdl_error();
        }
        
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

    int GM1EntryReader::CompatiblePixelFormat() const
    {
        return TGX::PixelFormatEnum;
    }
    
    uint32_t GM1EntryReader::GetColorKey() const
    {
        const uint8_t red = mTransparentColor.r;
        const uint8_t green = mTransparentColor.g;
        const uint8_t blue = mTransparentColor.b;
        const uint8_t alpha = mTransparentColor.a;

        const PixelFormatPtr fmt(SDL_AllocFormat(CompatiblePixelFormat()));
        const uint32_t color = SDL_MapRGBA(fmt.get(), red, green, blue, alpha);
        
        return color;
    }

    bool GM1EntryReader::Palettized() const
    {
        return SDL_ISPIXELFORMAT_INDEXED(CompatiblePixelFormat());
    }

    Color GM1EntryReader::Transparent() const
    {
        return mTransparentColor;
    }

    void GM1EntryReader::Transparent(Color color)
    {
        mTransparentColor = color;
    }

    Point GM1EntryReader::ImageCenter(const GM1::Header &header, const GM1::EntryHeader &entry) const
    {
        return Point(Width(entry) / 2, Height(entry) / 2);
    }
    
    GM1EntryReader::Ptr CreateEntryReader(const GM1::Encoding &encoding)
    {
        switch(encoding) {
        case GM1::Encoding::Font:
            return GM1EntryReader::Ptr(new FontReader);
            
        case GM1::Encoding::TGX16:
            return GM1EntryReader::Ptr(new TGX16);
                
        case GM1::Encoding::Bitmap:
            return GM1EntryReader::Ptr(new Bitmap);
        
        case GM1::Encoding::TGX8:
            return GM1EntryReader::Ptr(new TGX8);
            
        case GM1::Encoding::TileObject:
            return GM1EntryReader::Ptr(new TileObject);
            
        case GM1::Encoding::Unknown:
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }
    
}

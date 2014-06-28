#include "gm1entryreader.h"

#include <SDL.h>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <core/color.h>
#include <core/rect.h>
#include <core/palette.h>
#include <core/image.h>
#include <core/imageview.h>
#include <core/imagelocker.h>

#include <game/gm1reader.h>
#include <game/gm1.h>
#include <game/tgx.h>

namespace
{    
    /**
     * \brief Reader for animation sprites.
     *
     * 8-bit images;
     * tgx-compressed;
     */
    class TGX8 : public gm1::GM1EntryReader
    {
    protected:
        uint32_t SourcePixelFormat() const {
            return SDL_PIXELFORMAT_INDEX8;
        }

        void ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const;
    };

    /**
     * \brief Read for static textures.
     *
     * All fonts are such encoded.
     *
     * 16-bit images;
     * tgx-compressed;
     */
    class TGX16 : public gm1::GM1EntryReader
    {
    protected:
        void ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const;
    };

    class FontReader : public gm1::GM1EntryReader
    {
    protected:
        void ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const;
    };
    
    /**
     * \brief Reader for tile textures.
     *
     * Every tile is composed of 30x16 tile rhombus texture and
     * static TGX16 decoration.
     *
     * Tiles are decoded by tgx::DecodeTile
     *
     */
    class TileObject : public gm1::GM1EntryReader
    {
    protected:    
        void ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const;
    };

    /**
     * \tile Uncompressed images. 
     */
    class Bitmap : public gm1::GM1EntryReader
    {
    protected:
        int Height(const gm1::EntryHeader &header) const {
            // Nobody knows why
            return header.height - 7;
        }
    
        void ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const;
    };
    
    void TGX8::ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const
    {
        tgx::DecodeImage(in, numBytes, surface);
    }

    void TGX16::ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const
    {
        tgx::DecodeImage(in, numBytes, surface);
    }

    void FontReader::ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const
    {
        tgx::DecodeImage(in, numBytes, surface);

        // Originally I found that just color-keying of an image
        // doesn't work properly. After skipping all fully-transparent
        // pixels of the image some opaque magenta "halo" still remains.

        // In the way to solve it i'd just swapped green channel with alpha
        // and go clean my hands.

        // TODO is there a better way to do so?
        // uint32_t destFormat = SDL_PIXELFORMAT_ARGB8888;
        // core::Image tmp = core::ConvertImage(surface, destFormat);
        // tmp.SetColorKey(mTransparent);

        // Here we just ignore original color information. What we are really
        // interested in is green channel
        // auto swap_green_alpha = [](const core::Color &color) {
        //     return core::Color(color.r, 255, color.b, color.g);
        // };
    
        // castle::TransformImage(tmp, swap_green_alpha);

        // surface = tmp;
    }
    
    void Bitmap::ReadImage(std::istream &in, size_t numBytes, gm1::EntryHeader const&, core::Image &surface) const
    {
        core::ImageLocker lock(surface);

        const size_t stride = surface.RowStride();
        const size_t rowBytes = surface.Width() * surface.PixelStride();
        char *const data = lock.Data();

        for(size_t i = 0; (numBytes >= rowBytes) && (i < surface.Height()); ++i) {
            in.read(data + stride * i, rowBytes);
            numBytes -= rowBytes;
        }
    }
    
    // Width of rhombus rows in pixels.
    // \todo should it be placed in separate header file?
    constexpr uint8_t PerRow[] = {2, 6, 10, 14, 18, 22, 26, 30, 30, 26, 22, 18, 14, 10, 6, 2};
    
    constexpr uint8_t GetTilePixelsPerRow(size_t row)
    {
        return PerRow[row];
    }

    void ReadTile(std::istream &in, core::Image &image)
    {
        core::ImageLocker lock(image);
        
        const size_t rowStride = image.RowStride();
        const size_t height = gm1::TileSpriteHeight;
        const size_t width = gm1::TileSpriteWidth;
        const size_t pixelStride = image.PixelStride();
        char *data = lock.Data();
    
        for(size_t y = 0; y < height; ++y) {
            const size_t length = GetTilePixelsPerRow(y);
            const size_t offset = (width - length) / 2;
            in.read(data + offset * pixelStride, length * pixelStride);
            data += rowStride;
        }
    }
    
    void TileObject::ReadImage(std::istream &in, size_t numBytes, const gm1::EntryHeader &header, core::Image &surface) const
    {
        const core::Rect tilerect(0, header.tileY, Width(header), gm1::TileSpriteHeight);
        core::ImageView tile(surface, tilerect);
        ReadTile(in, tile.GetView());
        
        const core::Rect boxrect(header.hOffset, 0, header.boxWidth, Height(header));
        core::ImageView box(surface, boxrect);
        tgx::DecodeImage(in, numBytes - gm1::TileBytes, box.GetView());
    }
}

namespace gm1
{
    GM1EntryReader::GM1EntryReader()
        : mTransparentColor(255, 0, 255, 255)
    {
    }
    
    core::Image GM1EntryReader::CreateCompatibleImage(const gm1::EntryHeader &header) const
    {
        return core::CreateImage(Width(header), Height(header), SourcePixelFormat());
    }

    const core::Image GM1EntryReader::Load(const gm1::EntryHeader &header, const char *data, size_t bytesCount) const
    {
        core::Image image = CreateCompatibleImage(header);
        core::ClearImage(image, mTransparentColor);
        image.SetColorKey(mTransparentColor);
        boost::iostreams::stream<boost::iostreams::array_source> in(data, bytesCount);
        ReadImage(in, bytesCount, header, image);
        return image;
    }
    
    int GM1EntryReader::Width(const gm1::EntryHeader &header) const
    {
        return header.width;
    }

    int GM1EntryReader::Height(const gm1::EntryHeader &header) const
    {
        return header.height;
    }

    uint32_t GM1EntryReader::SourcePixelFormat() const
    {
        return tgx::PixelFormat;
    }

    uint32_t GM1EntryReader::GetColorKey(uint32_t format) const
    {
        return mTransparentColor.ConvertTo(format);
    }

    const core::Color GM1EntryReader::Transparent() const
    {
        return mTransparentColor;
    }

    void GM1EntryReader::Transparent(core::Color color)
    {
        mTransparentColor = std::move(color);
    }
    
    GM1EntryReader::Ptr CreateEntryReader(const ArchiveType &type)
    {
        switch(type) {
        case ArchiveType::Font:
            return GM1EntryReader::Ptr(new FontReader);
            
        case ArchiveType::TGX16:
            return GM1EntryReader::Ptr(new TGX16);
                
        case ArchiveType::Bitmap:
            return GM1EntryReader::Ptr(new Bitmap);
        
        case ArchiveType::TGX8:
            return GM1EntryReader::Ptr(new TGX8);
            
        case ArchiveType::TileObject:
            return GM1EntryReader::Ptr(new TileObject);
            
        case ArchiveType::Unknown:
        default:
            throw std::runtime_error("Unknown encoding");
        }
    }
}

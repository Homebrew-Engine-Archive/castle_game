#include "tgx.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <SDL.h>

#include "rw.h"
#include "surface.h"

namespace
{
    
    struct Header
    {
        uint32_t width;
        uint32_t height;
    };
    
    enum class TokenType : int
    {
        Stream = 0,
        Transparent = 1,
        Repeat = 2,
        LineFeed = 4
    };

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }

    void Overrun(const std::string &where, int64_t at)
    {
        std::ostringstream oss;
        oss << "In " << where << " overrun detected at " << at;
        throw std::runtime_error(oss.str());
    }

    void Overflow(const std::string &where, ptrdiff_t at)
    {
        std::ostringstream oss;
        oss << "In " << where << " overflow detected at " << at;
        throw std::runtime_error(oss.str());
    }
    
    // Width of rhombus rows in pixels.
    inline int GetTilePixelsPerRow(int row)
    {
        static const int PerRow[] = {
            2, 6, 10, 14, 18, 22, 26, 30, 30, 26, 22, 18, 14, 10, 6, 2
        };
        return PerRow[row];
    }

    Header ReadHeader(SDL_RWops *src)
    {
        Header hdr;
        hdr.width = SDL_ReadLE32(src);
        hdr.height = SDL_ReadLE32(src);
        return hdr;
    }
    
    size_t ExtractTokenLength(const uint8_t &token)
    {
        // Lower 5 bits represent length in range [1..32]
        return (token & 0x1f) + 1;
    }

    TokenType ExtractTokenType(uint8_t token)
    {
        // Higher 3 bits
        return static_cast<TokenType>(token >> 5);
    }

    std::string GetTokenTypeName(TokenType type)
    {
        switch(type) {
        case TokenType::Transparent: return "Transparent";
        case TokenType::Stream: return "Stream";
        case TokenType::LineFeed: return "LineFeed";
        case TokenType::Repeat: return "Repeat";
        default: return "Unknown";
        }
    }

    Surface CreateCompatibleSurface(int width, int height, int bpp)
    {
        uint32_t rmask = DefaultRedMask;
        uint32_t gmask = DefaultGreenMask;
        uint32_t bmask = DefaultBlueMask;
        uint32_t amask = DefaultAlphaMask;
        
        if(bpp == 16) {
            rmask = TGX::RedMask16;
            gmask = TGX::GreenMask16;
            bmask = TGX::BlueMask16;
            amask = TGX::AlphaMask16;
        }
    
        Surface surface = SDL_CreateRGBSurface(NoFlags, width, height, bpp, rmask, gmask, bmask, amask);
        if(surface.Null()) {
            Fail(__FUNCTION__, SDL_GetError());
        }
        return surface;
    }
    
    void RepeatPixel(const uint8_t *pixel, uint8_t *buff, size_t size, size_t count)
    {
        for(size_t i = 0; i < count; ++i) {
            std::copy(pixel, pixel + size, buff);
            buff += size;
        }
    }

    Surface LoadTGX(SDL_RWops *src, int64_t size, int width, int height, int bpp)
    {
        Surface surface = CreateCompatibleSurface(width, height, bpp);
        TGX::DecodeTGX(src, size, surface);
        return surface;
    }

}

namespace TGX
{

    Surface LoadStandaloneImage(SDL_RWops *src)
    {
        Header header = ReadHeader(src);
        return LoadTGX(src, ReadableBytes(src), header.width, header.height, 16);
    }

    void DecodeUncompressed(SDL_RWops *src, int64_t size, Surface &surface)
    {
        SurfaceLocker lock(surface);
        
        int bytesPerPixel = surface->format->BytesPerPixel;
        int widthBytes = surface->w * bytesPerPixel;    
        uint8_t *dst = reinterpret_cast<uint8_t*>(surface->pixels);
    
        int64_t npos = SDL_RWtell(src) + size;
        while(SDL_RWtell(src) + widthBytes <= npos) {
            SDL_RWread(src, dst, bytesPerPixel, surface->w);
            dst += surface->pitch;
        }
    }

    void DecodeTile(SDL_RWops *src, int64_t size, Surface &surface)
    {
        if(size < TileBytes) {
            Fail(__FUNCTION__, "Inconsistent tile size");
        }

        SurfaceLocker lock(surface);

        int pitchBytes = surface->pitch;
        size_t height = TileHeight;
        size_t width = TileWidth;
        uint8_t *dst = reinterpret_cast<uint8_t*>(surface->pixels);
        int bytesPerPixel = sizeof(uint16_t);
    
        for(size_t y = 0; y < height; ++y) {
            size_t length = GetTilePixelsPerRow(y);
            size_t offset = (width - length) / 2;
            SDL_RWread(src, dst + offset * bytesPerPixel, bytesPerPixel, length);
            dst += pitchBytes;
        }
    }

    void DecodeTGX(SDL_RWops *src, int64_t size, Surface &surface)
    {
        SurfaceLocker lock(surface);

        int bytesPerPixel = surface->format->BytesPerPixel;
        int bytesPitch = surface->pitch;
        int bytesDst = bytesPitch * surface->h;
        uint8_t *dst = reinterpret_cast<uint8_t*>(surface->pixels);
        uint8_t *dstBegin = dst;
        uint8_t *dstEnd = dstBegin + bytesDst;
        uint8_t *dstNextLine = dst + bytesPitch;
        int64_t npos = SDL_RWtell(src) + size;
        bool overrun = false;
        bool overflow = false;

        while(SDL_RWtell(src) < npos) {
            uint8_t token = SDL_ReadU8(src);
            TokenType type = ExtractTokenType(token);
            size_t length = ExtractTokenLength(token);
    
            switch(type) {
            case TokenType::LineFeed:
                {
                    if(length != 1) {
                        Fail(__FUNCTION__, "LineFeed token length should be 1");
                    }
                    if(dst > dstNextLine) {
                        Fail(__FUNCTION__, "dst is ahead of dstNextLine");
                    }
                    dst = dstNextLine;
                    dstNextLine += bytesPitch;
                }
                break;
            
            case TokenType::Transparent:
                {
                    if(dst < dstEnd) {
                        dst += length * bytesPerPixel;
                    } else {
                        overflow = true;
                    }
                }
                break;
            
            case TokenType::Stream:
                {
                    if(dst + length * bytesPerPixel <= dstEnd) {
                        if(SDL_RWread(src, dst, bytesPerPixel, length) == length) {
                            dst += length * bytesPerPixel;
                        } else {
                            overrun = true;
                        }
                    } else {
                        overflow = true;
                    }
                }
                break;

            case TokenType::Repeat:
                {
                    if(dst + length * bytesPerPixel <= dstEnd) {
                        // Read single pixel `length' times
                        if(SDL_RWread(src, dst, bytesPerPixel, 1) == 1) {
                            RepeatPixel(dst, dst, bytesPerPixel, length);
                            dst += length * bytesPerPixel;
                        } else {
                            overrun = true;
                        }
                    } else {
                        overflow = true;
                    }
                }
                break;

            default:
                {
                    Fail(__FUNCTION__, "Unknown token");
                }
                break;
            }

            if(overflow) {
                Overflow(__FUNCTION__, dst - dstBegin);
            }
            
            if(overrun) {
                Overrun(__FUNCTION__, SDL_RWtell(src));
            }
        }
    }

} // namespace tgx

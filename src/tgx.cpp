#include "tgx.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include "SDL.h"

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
        case TokenType::Transparent:
            return "Transparent";
        case TokenType::Stream:
            return "Stream";
        case TokenType::LineFeed:
            return "LineFeed";
        case TokenType::Repeat:
            return "Repeat";
        default:
            return "Unknown";
        }
    }

    void RepeatPixel(const uint8_t *pixel, uint8_t *buff, size_t size, size_t count)
    {
        for(size_t i = 0; i < count; ++i)
            std::copy(pixel, pixel + size, buff + i * size);
    }

    Surface LoadTGX(SDL_RWops *src, int64_t size, int width, int height, int bpp)
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
    
        Surface surface = SDL_CreateRGBSurface(
            NoFlags, width, height, bpp,
            rmask,
            gmask,
            bmask,
            amask);

        if(surface.Null()) {
            Fail("LoadTGX", SDL_GetError());
        }
    
        if(TGX::DecodeTGX(src, size, surface) < 0) {
            Fail("LoadTGX", "Unable to decode tgx");
        }
    
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

    int DecodeUncompressed(SDL_RWops *src, int64_t size, Surface &surface)
    {
        SurfaceLocker lock(surface);
    
        int bytesPerPixel = sizeof(uint16_t);
        int widthBytes = surface->w * bytesPerPixel;    
        uint8_t *dst = reinterpret_cast<uint8_t*>(surface->pixels);
    
        int64_t npos = SDL_RWtell(src) + size;
        while(SDL_RWtell(src) + widthBytes <= npos) {
            SDL_RWread(src, dst, bytesPerPixel, surface->w);
            dst += surface->pitch;
        }
    
        return 0;
    }

    int DecodeTile(SDL_RWops *src, int64_t size, Surface &surface)
    {
        if(ReadableBytes(src) < size) {
            Fail("DecodeTile", "EOF");
        }

        if(size < TileBytes) {
            Fail("DecodeTile", "Wrong size");
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
    
        return 0;
    }

    int DecodeTGX(SDL_RWops *src, int64_t size, Surface &surface)
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
                        std::cerr << "LineFeed token length should be 1"
                                  << std::endl;
                        return -1;
                    }
                    if(dst > dstNextLine) {
                        std::cerr << "dstNextLine ahead dst by "
                                  << std::dec
                                  << dst - dstNextLine
                                  << std::endl;
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
                    std::cerr << "Undefined token: "
                              << std::hex << token
                              << std::endl;
                    return -1;
                }
                break;
            }

            if(overflow) {
                std::cerr << "Overflow "
                          << std::dec
                          << "Token=" << GetTokenTypeName(type) << ','
                          << "Length=" << length << ','
                          << "Dst=" << dst - dstBegin << ','
                          << "Src=" << SDL_RWtell(src)
                          << std::endl;
                return -1;
            }
            
            if(overrun) {
                std::cerr << "Overrun "
                          << std::dec
                          << "Token=" << GetTokenTypeName(type) << ','
                          << "Length=" << length << ','
                          << "Dst=" << dst - dstBegin << ','
                          << "Src=" << SDL_RWtell(src)
                          << std::endl;
                return -1;
            }
        }
    
        return 0;
    }

} // namespace tgx

#include "tgx.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <SDL.h>
#include <stdexcept>

#include <boost/current_function.hpp>

#include "endianness.h"
#include "surface.h"

namespace
{
    
    struct Header
    {
        uint32_t width;
        uint32_t height;
    };

    /**
       TGX is just the sequence of tokens.
    **/
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

    std::istream& ReadHeader(std::istream &in, Header &header)
    {
        header.width = Endian::ReadLittle<uint32_t>(in);
        header.height = Endian::ReadLittle<uint32_t>(in);
        return in;
    }

    /**
     * Each token has its length. It is represented by lower 5 bits.
     * \note There are no 0 length.
     */
    size_t ExtractTokenLength(const uint8_t &token)
    {
        return (token & 0x1f) + 1;
    }

    /**
     * Each token has its type which are of type TokenType.
     * It is higher 3 bits.
     */
    TokenType ExtractTokenType(uint8_t token)
    {
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
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
        return surface;
    }
    
    void RepeatPixel(const char *pixel, char *buff, size_t size, size_t count)
    {
        for(size_t i = 0; i < count; ++i) {
            std::copy(pixel, pixel + size, buff);
            buff += size;
        }
    }
    
}

namespace TGX
{

    void DecodeTile(std::istream &in, size_t numBytes, Surface &surface)
    {
        if(numBytes < TileBytes) {
            Fail(BOOST_CURRENT_FUNCTION, "Inconsistent tile size");
        }

        const SurfaceLocker lock(surface);

        const int pitchBytes = surface->pitch;
        const int height = TileHeight;
        const int width = TileWidth;
        const int bytesPerPixel = surface->format->BytesPerPixel;
        char *dst = reinterpret_cast<char *>(surface->pixels);
    
        for(int y = 0; y < height; ++y) {
            const int length = GetTilePixelsPerRow(y);
            const int offset = (width - length) / 2;
            in.read(dst + offset * bytesPerPixel, bytesPerPixel * length);
            dst += pitchBytes;
        }
    }

    void DecodeBitmap(std::istream &in, size_t numBytes, Surface &surface)
    {
        const SurfaceLocker lock(surface);

        const int bytesPerPixel = surface->format->BytesPerPixel;
        const size_t rowBytes = surface->w * bytesPerPixel;    
        char *dst = reinterpret_cast<char *>(surface->pixels);
    
        while(numBytes >= rowBytes) {
            in.read(dst, rowBytes);
            dst += surface->pitch;
            numBytes -= rowBytes;
        }
    }

    Surface LoadStandaloneImage(std::istream &in)
    {
        Header header;
        if(!ReadHeader(in, header)) {
            Fail(BOOST_CURRENT_FUNCTION, "Can't read header");
        }
        Surface surface = CreateCompatibleSurface(header.width, header.height, 16);

        std::streampos origin = in.tellg();
        in.seekg(0, std::ios_base::end);
        std::streampos fsize = in.tellg();
        in.seekg(origin);
        
        TGX::DecodeTGX(in, fsize - origin, surface);
        return surface;
    }

    void DecodeTGX(std::istream &in, size_t numBytes, Surface &surface)
    {
        const SurfaceLocker lock(surface);
        
        std::streampos npos = numBytes + in.tellg();
        const int bytesPitch = surface->pitch;
        char *dst = reinterpret_cast<char *>(surface->pixels);
        char *dstNextLine = dst + bytesPitch;
        const char *dstBegin = dst;
        const char *dstEnd = dstBegin + bytesPitch * surface->h;
        const int bytesPerPixel = surface->format->BytesPerPixel;
        bool overflow = false;
        
        while(in.tellg() < npos) {
            const uint8_t token = Endian::ReadLittle<uint8_t>(in);
            const TokenType type = ExtractTokenType(token);
            const size_t length = ExtractTokenLength(token);
            
            switch(type) {
            case TokenType::LineFeed:
                {
                    if(length != 1) {
                        Fail(BOOST_CURRENT_FUNCTION, "LineFeed token length should be 1");
                    }
                    if(dst > dstNextLine) {
                        Fail(BOOST_CURRENT_FUNCTION, "Destination pointer is ahead of next line pointer");
                    }
                    // TODO may it should be checked for overflow too?
                    dst = dstNextLine;
                    dstNextLine += bytesPitch;
                }
                break;
            
            case TokenType::Transparent:
                {
                    if(dst + length * bytesPerPixel <= dstEnd) {
                        dst += length * bytesPerPixel;
                    } else {
                        overflow = true;
                    }
                }
                break;
            
            case TokenType::Stream:
                {
                    if(dst + length * bytesPerPixel <= dstEnd) {
                        if(in.read(dst, bytesPerPixel * length)) {
                            dst += length * bytesPerPixel;
                        }
                    } else {
                        overflow = true;
                    }
                }
                break;

            case TokenType::Repeat:
                {
                    if(dst + length * bytesPerPixel <= dstEnd) {
                        // Read single pixel N times
                        if(in.read(dst, bytesPerPixel)) {
                            RepeatPixel(dst, dst, bytesPerPixel, length);
                            dst += length * bytesPerPixel;
                        }
                    } else {
                        overflow = true;
                    }
                }
                break;

            default:
                {
                    Fail(BOOST_CURRENT_FUNCTION, "Unknown token");
                }
                break;
            }
            
            if(overflow) {
                Overflow(BOOST_CURRENT_FUNCTION, dst - dstBegin);
            }

            if(in.bad()) {
                Overrun(BOOST_CURRENT_FUNCTION, in.tellg());
            }
        }
    }
    
} // namespace TGX

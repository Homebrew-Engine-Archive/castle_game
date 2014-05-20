#include "tgx.h"

#include <cassert>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include <SDL.h>

#include <boost/current_function.hpp>

#include <game/color.h>
#include <game/iohelpers.h>
#include <game/surface.h>

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

    typedef uint8_t token_t;

    const int MaxTokenLength = 32;

    constexpr int ExtractTokenLength(token_t token)
    {
        return (token & 0x1f) + 1;
    }

    constexpr TokenType ExtractTokenType(token_t token)
    {
        return static_cast<TokenType>(token >> 5);
    }

    constexpr token_t MakeToken(TokenType type, int length)
    {
        return ((static_cast<int>(type) & 0x0f) << 5) | ((length - 1) & 0x1f);
    }

    constexpr token_t MakeStreamToken(int length)
    {
        return MakeToken(TokenType::Stream, std::move(length));
    }

    constexpr token_t MakeRepeatToken(int length)
    {
        return MakeToken(TokenType::Repeat, std::move(length));
    }

    constexpr token_t MakeTransparentToken(int length)
    {
        return MakeToken(TokenType::Transparent, std::move(length));
    }

    constexpr token_t MakeLineFeedToken()
    {
        return MakeToken(TokenType::LineFeed, 1);
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
        
    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
    
    std::istream& ReadHeader(std::istream &in, Header &header)
    {
        io::ReadLittle(in, header.width);
        io::ReadLittle(in, header.height);
        return in;
    }

    std::ostream& WriteHeader(std::ostream &out, const Header &header)
    {
        io::WriteLittle(out, header.width);
        io::WriteLittle(out, header.height);
        return out;
    }
    
    Surface CreateCompatibleSurface(int width, int height)
    {
        Surface surface = CreateSurface(width, height, TGX::PixelFormatEnum);
        if(!surface) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }

        return surface;
    }
    
    bool PixelsEqual(const char *lhs, const char *rhs, int bytesPerPixel)
    {
        return std::equal(lhs, lhs + bytesPerPixel, rhs);
    }

    int PixelsCount(const char *lhs, const char *rhs, int bytesPerPixel)
    {
        return std::distance(lhs, rhs) / bytesPerPixel;
    }

    bool PixelTransparent(const char *pixels, uint32_t colorKey, int bytesPerPixel)
    {
        return GetPackedPixel(pixels, bytesPerPixel) == colorKey;
    }
    
    std::ostream& WriteStreamToken(std::ostream &out, const char *pixels, int numPixels, int bytesPerPixel)
    {
        if(numPixels > 0) {
            io::WriteLittle(out, MakeStreamToken(numPixels));
            return out.write(pixels, numPixels * bytesPerPixel);
        }

        return out;
    }

    std::ostream& WriteLineFeed(std::ostream &out)
    {
        return io::WriteLittle(out, MakeLineFeedToken());
    }

    std::ostream& WriteTransparentToken(std::ostream &out, int numPixels)
    {
        if(numPixels > 0) {
            return io::WriteLittle(out, MakeTransparentToken(numPixels));
        }

        return out;
    }
    std::ostream& WriteRepeatToken(std::ostream &out, const char *pixels, int numPixels, int bytesPerPixel)
    {
        if(numPixels > 0) {
            io::WriteLittle(out, MakeRepeatToken(numPixels));
            return out.write(pixels, bytesPerPixel);
        }

        return out;
    }
}

namespace TGX
{

    std::ostream& EncodeLine(std::ostream &out, const char *pixels, int width, int bytesPP, uint32_t colorKey)
    {
        const char *end = pixels + width * bytesPP;

        while(pixels != end) {
            /** Grab all transparent pixels **/
            {
                const char *mark = pixels;
                while((pixels != end) &&
                      (PixelTransparent(pixels, colorKey, bytesPP)) &&
                      (PixelsCount(mark, pixels, bytesPP) < MaxTokenLength)) {
                    pixels += bytesPP;
                }
                WriteTransparentToken(out, PixelsCount(mark, pixels, bytesPP));
            }

            /** Grab two or more repeating pixels **/
            {
                const char *mark = pixels;
                while((pixels != end) && 
                      (!PixelTransparent(pixels, colorKey, bytesPP)) &&
                      (PixelsEqual(pixels, mark, bytesPP)) && 
                      (PixelsCount(mark, pixels, bytesPP) < MaxTokenLength)) {
                    pixels += bytesPP;
                }
                if(PixelsCount(mark, pixels, bytesPP) == 1) {
                    pixels -= bytesPP;
                }
                
                WriteRepeatToken(out, mark, PixelsCount(mark, pixels, bytesPP), bytesPP);
            }

            /** Grab all the rest **/
            {
                const char *mark = pixels;
                while((pixels != end) && 
                      (!PixelTransparent(pixels, colorKey, bytesPP)) &&
                      ((pixels == mark) || (!PixelsEqual(pixels, pixels - bytesPP, bytesPP))) &&
                      (PixelsCount(mark, pixels, bytesPP) < MaxTokenLength)) {
                    pixels += bytesPP;
                }
                if((pixels != mark) && (PixelsEqual(pixels, pixels - bytesPP, bytesPP))) {
                    pixels -= bytesPP;
                }

                WriteStreamToken(out, mark, PixelsCount(mark, pixels, bytesPP), bytesPP);
            }
        }
        
        return WriteLineFeed(out);
    }    

    std::ostream& EncodeSurface(std::ostream &out, const Surface &surface)
    {
        assert(!surface.Null());
        assert(surface->format != NULL);
        assert(!out.fail());
        assert(!out.bad());

        const SurfaceLocker lock(surface);
        
        const char *pixelsPtr = ConstGetPixels(surface);
        const int bytesPerPixel = surface->format->BytesPerPixel;

        uint32_t colorKey = 0;
        if(SDL_GetColorKey(surface, &colorKey) < 0) {
            std::cerr << "No color key in surface" << std::endl;
        }

        for(int row = 0; row < surface->h; ++row) {
            EncodeLine(out, pixelsPtr, surface->w, bytesPerPixel, colorKey);
            if(!out) {
                return out;
            }
            pixelsPtr += surface->pitch;
        }
            
        return out;
    }
    
    std::ostream& WriteTGX(std::ostream &out, const Surface &surface)
    {
        Header header;
        header.width = surface->w;
        header.height = surface->h;
        WriteHeader(out, header);
        return EncodeSurface(out, surface);
    }
    
    Surface ReadTGX(std::istream &in)
    {
        Header header;
        if(!ReadHeader(in, header)) {
            Fail(BOOST_CURRENT_FUNCTION, "Can't read header");
        }
        Surface surface = CreateCompatibleSurface(header.width, header.height);

        const std::streampos origin = in.tellg();
        in.seekg(0, std::ios_base::end);
        const std::streampos fsize = in.tellg();
        in.seekg(origin);
        
        TGX::DecodeSurface(in, fsize - origin, surface);
        return surface;
    }

    std::istream& DecodeLine(std::istream &in, size_t numBytes, char *dst, size_t width, size_t bytesPerPixel)
    {
        const std::streampos endPos = numBytes + in.tellg();
        const char *dstEnd = dst + width * bytesPerPixel;
        
        while(in.tellg() < endPos) {
            token_t token;
            io::ReadLittle(in, token);
            // \note any io errors are just ignored if token has valid TokenType
            // we handle them latter anyway

            const TokenType type = ExtractTokenType(token);
            const int length = ExtractTokenLength(token);
            
            switch(type) {
            case TokenType::Repeat:
            case TokenType::Stream:
            case TokenType::Transparent:
                {
                    // \todo what if new dst value is equal to dstEnd? In that case
                    // we have no space for placing LineFeed. It is certainly an erroneous behavior.
                    // Should we report it here?
                    if(dst + length * bytesPerPixel > dstEnd) {
                        Fail(BOOST_CURRENT_FUNCTION, "Overflow");
                    }
                }
            default:
                break;
            }
            
            switch(type) {
            case TokenType::LineFeed:
                {
                    if(length != 1) {
                        Fail(BOOST_CURRENT_FUNCTION, "Inconsistent line feed");
                    }
                    return in;
                }
                break;
                
            case TokenType::Repeat:
                {
                    in.read(dst, bytesPerPixel);
                    for(int n = 0; n < length; ++n) {
                        std::copy(dst, dst + bytesPerPixel, dst + n * bytesPerPixel);
                    }
                }
                break;
                
            case TokenType::Stream:
                {
                    in.read(dst, length * bytesPerPixel);
                }
                break;
                
            case TokenType::Transparent:
                break;
                
            default:
                {
                    if(!in) {
                        Fail(BOOST_CURRENT_FUNCTION, "Unable to read token");
                    }
                    Fail(BOOST_CURRENT_FUNCTION, "Unknown token");
                }
            }

            if(!in) {
                Fail(BOOST_CURRENT_FUNCTION, strerror(errno));
            }
            
            dst += length * bytesPerPixel;
        }
        
        return in;
    }
    
    std::istream& DecodeSurface(std::istream &in, size_t numBytes, Surface &surface)
    {
        const SurfaceLocker lock(surface);

        const int width = surface->w;
        const int height = surface->h;
        const int bytesPP = surface->format->BytesPerPixel;
        const int pitch = surface->pitch;

        const std::streampos endPos = numBytes + in.tellg();
        
        char *dst = GetPixels(surface);

        for(int row = 0; row < height; ++row) {
            if((in) && (in.tellg() < endPos)) {
                size_t bytesLeft = endPos - in.tellg();
                DecodeLine(in, bytesLeft, dst, width, bytesPP);
                dst += pitch;
            }
        }

        // Make a look that we read all numBytes
        if(in) {
            in.seekg(endPos);
        }

        return in;
    }
    
    std::istream& ReadSurfaceHeader(std::istream &in, Surface &surface)
    {
        Header header;
        if(!ReadHeader(in, header)) {
            Fail(BOOST_CURRENT_FUNCTION, "Can't read header");
        }
        surface = CreateCompatibleSurface(header.width, header.height);
        return in;
    }
    
} // namespace TGX 

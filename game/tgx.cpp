#include "tgx.h"

#include <cassert>
#include <cstring>

#include <functional>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include <core/color.h>
#include <core/image.h>
#include <core/imagelocker.h>
#include <core/iohelpers.h>

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
    
    std::istream& ReadHeader(std::istream &in, Header &header)
    {
        core::ReadLittle(in, header.width);
        core::ReadLittle(in, header.height);
        return in;
    }

    std::ostream& WriteHeader(std::ostream &out, const Header &header)
    {
        core::WriteLittle(out, header.width);
        core::WriteLittle(out, header.height);
        return out;
    }
    
    core::Image CreateCompatibleImage(int width, int height)
    {
        return core::CreateImage(width, height, tgx::PixelFormat);
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
        return core::GetPackedPixel(pixels, bytesPerPixel) == colorKey;
    }
    
    std::ostream& WriteStreamToken(std::ostream &out, const char *pixels, int numPixels, int bytesPerPixel)
    {
        if(numPixels > 0) {
            core::WriteLittle(out, MakeStreamToken(numPixels));
            return out.write(pixels, numPixels * bytesPerPixel);
        }

        return out;
    }

    std::ostream& WriteLineFeed(std::ostream &out)
    {
        return core::WriteLittle(out, MakeLineFeedToken());
    }

    std::ostream& WriteTransparentToken(std::ostream &out, int numPixels)
    {
        if(numPixels > 0) {
            return core::WriteLittle(out, MakeTransparentToken(numPixels));
        }

        return out;
    }
    std::ostream& WriteRepeatToken(std::ostream &out, const char *pixels, int numPixels, int bytesPerPixel)
    {
        if(numPixels > 0) {
            core::WriteLittle(out, MakeRepeatToken(numPixels));
            return out.write(pixels, bytesPerPixel);
        }

        return out;
    }
}

namespace tgx
{
    template<class TransparencyPred>
    std::ostream& EncodeLine(std::ostream &out, const char *pixels, int width, int bytesPP, TransparencyPred transparent)
    {
        const char *end = pixels + width * bytesPP;

        while(pixels != end) {
            /** Grab all transparent pixels **/
            {
                const char *mark = pixels;
                while((pixels != end) &&
                      (transparent(pixels)) &&
                      (PixelsCount(mark, pixels, bytesPP) < MaxTokenLength)) {
                    pixels += bytesPP;
                }
                WriteTransparentToken(out, PixelsCount(mark, pixels, bytesPP));
            }

            /** Grab two or more repeating pixels **/
            {
                const char *mark = pixels;
                while((pixels != end) && 
                      (!transparent(pixels)) &&
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
                      (!transparent(pixels)) &&
                      ((pixels == mark) || (!PixelsEqual(pixels, pixels - bytesPP, bytesPP))) &&
                      (PixelsCount(mark, pixels, bytesPP) < MaxTokenLength)) {
                    pixels += bytesPP;
                }
                if((pixels != end) && (pixels != mark) && (PixelsEqual(pixels, pixels - bytesPP, bytesPP))) {
                    pixels -= bytesPP;
                }

                WriteStreamToken(out, mark, PixelsCount(mark, pixels, bytesPP), bytesPP);
            }
        }
        
        return WriteLineFeed(out);
    }    

    std::ostream& EncodeImage(std::ostream &out, const core::Image &image)
    {
        const core::ImageLocker lock(image);
        
        const char *data = lock.Data();
        const auto pixelStride = image.PixelStride();
        const auto rowStride = image.RowStride();

        std::function<bool(const char*)> transparencyPredicate = [](const char *pixel) {
            return false;
        };

        if(image.ColorKeyEnabled()) {
            const uint32_t colorKey = image.GetColorKey().ConvertTo(core::ImageFormat(image));
            transparencyPredicate = [pixelStride, colorKey](const char *pixel) {
                return PixelTransparent(pixel, colorKey, pixelStride);
            };
        }

        for(size_t y = 0; y < image.Height(); ++y) {
            EncodeLine(out, data + rowStride * y, image.Width(), pixelStride, transparencyPredicate);
            if(!out) {
                return out;
            }
        }
            
        return out;
    }
    
    std::ostream& WriteImage(std::ostream &out, const core::Image &surface)
    {
        Header header;
        header.width = surface.Width();
        header.height = surface.Height();
        WriteHeader(out, header);
        return EncodeImage(out, surface);
    }
    
    const core::Image ReadImage(std::istream &in)
    {
        Header header;
        if(!ReadHeader(in, header)) {
            throw std::runtime_error(strerror(errno));
        }

        const std::streampos origin = in.tellg();
        in.seekg(0, std::ios_base::end);
        const std::streampos fsize = in.tellg();
        in.seekg(origin);

        core::Image surface = CreateCompatibleImage(header.width, header.height);
        tgx::DecodeImage(in, fsize - origin, surface);
        
        return surface;
    }

    std::istream& DecodeLine(std::istream &in, size_t numBytes, char *data, size_t width, size_t bytesPerPixel)
    {
        const std::streampos endPos = numBytes + in.tellg();
        const char *const dataEnd = data + width * bytesPerPixel;
        
        while(in.tellg() < endPos) {
            token_t token;
            core::ReadLittle(in, token);
            // \note any io errors are just ignored if token has valid TokenType
            // but we handle them latter anyway

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
                    if(data + length * bytesPerPixel > dataEnd) {
                        throw std::overflow_error("token length exceeds available buffer size");
                    }
                }
            default:
                break;
            }
            
            switch(type) {
            case TokenType::LineFeed:
                {
                    if(length != 1) {
                        throw std::logic_error("inconsistent line break");
                    }
                    return in;
                }
                break;
                
            case TokenType::Repeat:
                {
                    in.read(data, bytesPerPixel);
                    for(int n = 0; n < length; ++n) {
                        std::copy(data, data + bytesPerPixel, data + n * bytesPerPixel);
                    }
                }
                break;
                
            case TokenType::Stream:
                {
                    in.read(data, length * bytesPerPixel);
                }
                break;
                
            case TokenType::Transparent:
                break;
                
            default:
                {
                    if(!in) {
                        throw std::runtime_error(strerror(errno));
                    }
                    throw std::logic_error("unknown tgx token type");
                }
            }

            if(!in) {
                throw std::runtime_error(strerror(errno));
            }
            
            data += length * bytesPerPixel;
        }
        
        return in;
    }
    
    std::istream& DecodeImage(std::istream &in, size_t numBytes, core::Image &image)
    {
        core::ImageLocker lock(image);

        const int width = image.Width();
        const int height = image.Height();
        const int pixelStride = image.PixelStride();
        const int rowStride = image.RowStride();

        const std::streampos endPos = numBytes + in.tellg();
        
        char *const data = lock.Data();

        for(int y = 0; y < height; ++y) {
            if((in) && (in.tellg() < endPos)) {
                size_t bytesLeft = endPos - in.tellg();
                DecodeLine(in, bytesLeft, data + rowStride * y, width, pixelStride);
            }
        }

        // Make a look that we read all numBytes
        if(in) {
            in.seekg(endPos);
        }

        return in;
    }
    
    std::istream& ReadImageHeader(std::istream &in, core::Image &surface)
    {
        Header header;
        if(!ReadHeader(in, header)) {
            throw std::runtime_error(strerror(errno));
        }
        surface = CreateCompatibleImage(header.width, header.height);
        return in;
    }
    
} // namespace tgx 

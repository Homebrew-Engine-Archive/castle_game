#ifndef TGX_H_
#define TGX_H_

#include <SDL.h>

#include <cstdint>
#include <iosfwd>

/**
 * TGX is a RLE-like compression algorithm.
 *
 * The data are splitted up into triples
 *
 *          <type> <length> <data>
 *
 * where <type> has size of 3 bits
 *       <length> has size of 5 bits
 *       and data has size up to <length> pixels
 *
 * Each pixel in data has fixed size either 16 or 8 bits (see gm1)
 *
 * \note <type> and <length> are combined in a single byte (obvious, isn't it?)
 *
 * \note <length> has value in range [1..32] (not in [0..31])
 *
 * <type> represents how the data are stored.
 * It might be either just pixels (value 0)
 *                 or translucent pixels (value 1)
 *                 or repeated pixel (value 2)
 *                 or newline token (value 4)
 *
 * Type 0 token followed by just array of pixels.
 * Type 1 token has no followed data. It's  color
 *        are defined above this notice.  
 * Type 2 token has single pixel which are copied many times.
 * Type 4 token always has length 1 but no data,
 *        it ends up each line of image.
 *
 * Type is represented by TokenType enumeration
 * which are defined in cpp.
 *
 */

namespace castle
{
    class Image;
}

namespace TGX
{
    /**
       Stronghold's gfx transparentcy is pain.
       
       Consider simple ARGB1555 magenta color
            11111100 00011111 (0x7C1F)
       and this is what I found in stronghold gfx
            11111000 00011111 (0xF81F) 
      
       Say it is RGB565? Nope, it's not!
       Almost everything in SH gfx can be decoded using just Little-Endian, RGB555 and
       0xf81f as transparency. Almost, except font glyphs,
       some buildings, puff of smoke and something other.
       In this cases some pixels have weak magenta tint, but
       they aren't all the same.
       
       \todo bring all such cases here to analyze.

       Some constants I mention earlier:
       
       const uint16_t Transparent16 = 0xF81F;
      
       const uint32_t AlphaMask16  = 0x00008000;
       const uint32_t RedMask16    = 0x00007c00;
       const uint32_t GreenMask16  = 0x000003e0;
       const uint32_t BlueMask16   = 0x0000001f;
    **/

    constexpr uint32_t PixelFormat = SDL_PIXELFORMAT_RGB555;
    
    std::istream& DecodeLine(std::istream&, size_t numBytes, char *dst, size_t width, size_t bytesPerPixel);
    
    std::istream& DecodeImage(std::istream&, size_t numBytes, castle::Image &surface);

    std::istream& ReadImageHeader(std::istream&, castle::Image &surface);

    const castle::Image ReadTGX(std::istream&);

    /**
     * \brief Low level TGX-encoding function.
     *
     * \param out           Output stream.
     * \param pixels        Input buffer.
     * \param width         Buffer size in pixels.
     * \param bytesPP       Number of bytes per pixel.
     * \param colorKey      Pixel which we would treat as transparent.
     *
     * \note Input buffer must have real size of width * bytesPerPixel bytes.
     **/
    // std::ostream& EncodeLine(std::ostream&, const char *pixels, int width, int bytesPP, uint32_t colorKey);
    
    std::ostream& EncodeImage(std::ostream&, const castle::Image &surface);

    std::ostream& WriteImageHeader(std::ostream&, const castle::Image &surface);

    std::ostream& WriteTGX(std::ostream&, const castle::Image &surface);
    
} // namespace TGX

#endif

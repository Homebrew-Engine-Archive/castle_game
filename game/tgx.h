#ifndef TGX_H_
#define TGX_H_

#include <cstdint>
#include <iosfwd>

/**
 * TGX is a RLE-like compression algorithm.
 *
 * The data are splitted up into triples
 *
 *          <type> <length> <data>
 *
 * where type has size of 3 bits
 *       length has size of 5 bits
 *       and data has size up to <length> pixels
 *
 * Each pixel in data has fixed size either 16 or 8 bits (see gm1)
 *
 * \note type and length are combined in a single byte (obvious, isn't it?)
 *
 * \note length has value in range [1..32] (not in [0..31])
 *
 * Type represents how the data are stored.
 * It might be either just pixels (value 0)
 *                 or translucent pixels (value 1)
 *                 or repeated pixel (value 2)
 *                 or newline token (value 4)
 *
 * Type 0 token followed by just array of pixels.
 * Type 1 token has no followed data. It's transparent color
 *        are defined above this notice.  
 * Type 2 token has single pixel which are copied many times.
 * Type 4 token always has length 1 but no data,
 *        it ends up each line of image.
 *
 * Type is represented by TokenType enumeration
 * which are defined in cpp.
 *
 */

class Surface;

namespace TGX
{
    
    /**
     * Magenta color as 16 bit transparency.
     * Actually there are no need in the value of such color.
     * \note It is not 1 11111 00000 11111 (0x7C1F)
     *            it's 1 11110 00000 11111
     */
    const uint16_t Transparent16 = 0xF81F;
    
    // Palette's first entry
    const uint8_t Transparent8 = 0;
    
    // Five bits for the RED alert under the uporin
    // Five for the GREEN tea in its pointless being
    // Five for mortal BLUE
    // Doomed to be blue
    // And one bit - for the ALPHA lord on its alpha throne
    // In the land of sprites where some shadows was
    // One bit to rule them all, one bit to find them
    // One bit to bring them all, and in the ARGB bind them
    // In the land of sprites where some shadows was

    /// \note This masks are not swaped neither according to endianness
    const uint32_t AlphaMask16  = 0x00008000;
    const uint32_t RedMask16    = 0x00007c00;
    const uint32_t GreenMask16  = 0x000003e0;
    const uint32_t BlueMask16   = 0x0000001f;

    uint32_t GetPixelFormatEnum();
    
    std::istream& DecodeBuffer(std::istream&, size_t numBytes, char *dst, size_t width, size_t bytesPerPixel);
    
    std::istream& DecodeSurface(std::istream&, size_t numBytes, Surface &surface);

    std::istream& ReadSurfaceHeader(std::istream &, Surface &surface);

    Surface ReadTGX(std::istream&);

    /**
     * \brief Main state machine for encoding sequences of pixels.
     *
     * \param out           Output stream.
     * \param pixels        Input buffer.
     * \param width         Buffer size in pixels.
     * \param bytesPerPixel Pixel size in bytes.
     *
     * \note Input buffer have real size of width * bytesPerPixel
     */
    std::ostream& EncodeBuffer(std::ostream&, const char *pixels, int width, int bytesPerPixel);
    
    std::ostream& EncodeSurface(std::ostream&, const Surface &surface);

    std::ostream& WriteTGX(std::ostream&, const Surface &surface);
    
    //std::ostream& WriteTGX(std::ostream&, int width, int height, char const*, size_t numBytes);

    //std::ostream& WriteSurface(std::ostream&, const Surface &surface);

    std::ostream& WriteSurfaceHeader(std::ostream&, const Surface &surface);
    
} // namespace TGX

#endif

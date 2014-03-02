#include "tgx.h"

NAMESPACE_BEGIN(tgx)

static const char *GetTokenTypeName(TokenType);
static TokenType ExtractTokenType(const Uint8 &token);
static size_t ExtractTokenLength(const Uint8 &token);
static int ReadHeader(SDL_RWops *src, Header *header);

Surface LoadStandaloneImage(SDL_RWops *src)
{
    Header header;
    if(ReadHeader(src, &header) < 0) {
        std::cerr << "ReadHeader failed" << std::endl;
        return Surface();
    }

    return LoadTGX(src, ReadableBytes(src), header.width, header.height, 16);
}

static int ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < 2 * sizeof(Uint32)) {
        std::cerr << "Buffer overrun" << std::endl;
        return -1;
    }
    
    hdr->width = SDL_ReadLE32(src);
    hdr->height = SDL_ReadLE32(src);
    return 0;
}

int DecodeUncompressed(SDL_RWops *src, Sint64 size, Surface &surface)
{
    SurfaceLocker lock(surface);
    
    int bytesPerPixel = sizeof(Uint16);
    int widthBytes = surface->w * bytesPerPixel;    
    Uint8 *dst = reinterpret_cast<Uint8*>(surface->pixels);
    
    Sint64 npos = SDL_RWtell(src) + size;
    while(SDL_RWtell(src) + widthBytes <= npos) {
        SDL_RWread(src, dst, bytesPerPixel, surface->w);
        dst += surface->pitch;
    }
    
    return 0;
}

int DecodeTile(SDL_RWops *src, Sint64 size, Surface &surface)
{
    if(ReadableBytes(src) < size) {
        std::cerr << "Source buffer overrun" << std::endl;
        return -1;
    }

    if(size < TILE_BYTES) {
        std::cerr << "Incorrect size" << std::endl;
        return -1;
    }

    SurfaceLocker lock(surface);

    int pitchBytes = surface->pitch;
    size_t height = TILE_RHOMBUS_HEIGHT;
    size_t width = TILE_RHOMBUS_WIDTH;
    Uint8 *dst = reinterpret_cast<Uint8*>(surface->pixels);
    int bytesPerPixel = sizeof(Uint16);
    
    for(size_t y = 0; y < height; ++y) {
        size_t length = TILE_PIXELS_PER_ROW[y];
        size_t offset = (width - length) / 2;

        SDL_RWread(src, dst + offset * bytesPerPixel, bytesPerPixel, length);

        dst += pitchBytes;
    }
    
    return 0;
}

static size_t ExtractTokenLength(const Uint8 &token)
{
    // Lower 5 bits represent range between [1..32]
    return (token & 0x1f) + 1;
}

static TokenType ExtractTokenType(const Uint8 &token)
{
    // Higher 3 bits
    return TokenType(token >> 5);
}

static const char * GetTokenTypeName(TokenType type)
{
    UNUSED(GetTokenTypeName);
    switch(type) {
    case TokenType::Transparent: return "Transparent";
    case TokenType::Stream: return "Stream";
    case TokenType::LineFeed: return "LineFeed";
    case TokenType::Repeat: return "Repeat";
    default:
        return "Unknown";
    }
}

static void RepeatPixel(const Uint8 *pixel, Uint8 *buff, size_t size, size_t num)
{
    for(size_t i = 0; i < num; ++i)
        std::copy(pixel, pixel + size, buff + i * size);
}

Surface LoadTGX(SDL_RWops *src, Sint64 size, int width, int height, int bpp)
{
    Uint32 rmask = RMASK_DEFAULT;
    Uint32 gmask = GMASK_DEFAULT;
    Uint32 bmask = BMASK_DEFAULT;
    Uint32 amask = AMASK_DEFAULT;
    if(bpp == 16) {
        rmask = TGX_RGB16_RMASK;
        gmask = TGX_RGB16_GMASK;
        bmask = TGX_RGB16_BMASK;
        amask = TGX_RGB16_AMASK;
    }
    
    Surface surface = SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);

    if(surface.Null()) {
        std::cerr << "SDL_CreateRGBSurface failed: ";
        std::cerr << SDL_GetError();
        std::cerr << std::endl;
        return Surface();
    }
    
    if(DecodeTGX(src, size, surface) < 0) {
        std::cerr << "DecodeTGX failed" << std::endl;
        return Surface();
    }
    
    return surface;
}

int DecodeTGX(SDL_RWops *src, Sint64 size, Surface &surface)
{
    SurfaceLocker lock(surface);

    int bytesPerPixel = surface->format->BytesPerPixel;
    int bytesPitch = surface->pitch;
    int bytesDst = bytesPitch * surface->h;
    Uint8 *dst = reinterpret_cast<Uint8*>(surface->pixels);
    Uint8 *dstBegin = dst;
    Uint8 *dstEnd = dst + bytesDst;
    Uint8 *dstNextLine = dst + bytesPitch;
    Sint64 npos = SDL_RWtell(src) + size;
    bool overrun = false;
    bool overflow = false;

    while(SDL_RWtell(src) < npos) {
        Uint8 token = SDL_ReadU8(src);
        TokenType type = ExtractTokenType(token);
        size_t length = ExtractTokenLength(token);

        switch(type) {
        case TokenType::LineFeed:
            {
                if(length != 1) {
                    std::cerr << "LineFeed token length should be 1" << std::endl;
                    return -1;
                }
                if(dst > dstNextLine) {
                    std::cerr << "dstNextLine ahead dst by ";
                    std::cerr << std::dec;
                    std::cerr << (dst - dstNextLine);
                    std::cerr << std::endl;
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
                std::cerr << "Undefined token: ";
                std::cerr << std::hex << token;
                std::cerr << std::endl;
                return -1;
            }
            break;
        }

        if(overflow) {
            std::cerr << "Overflow ";
            std::cerr << std::dec;
            std::cerr << "Token=" << GetTokenTypeName(type) << ',';
            std::cerr << "Length=" << length << ',';
            std::cerr << "Dst=" << dst - dstBegin << ',';
            std::cerr << "Src=" << SDL_RWtell(src);
            std::cerr << std::endl;
            return -1;
        }
        if(overrun) {
            std::cerr << "Overrun ";
            std::cerr << std::dec;
            std::cerr << "Token=" << GetTokenTypeName(type) << ',';
            std::cerr << "Length=" << length << ',';
            std::cerr << "Dst=" << dst - dstBegin << ',';
            std::cerr << "Src=" << SDL_RWtell(src);
            std::cerr << std::endl;
            return -1;
        }
    }
    
    return 0;
}

NAMESPACE_END(tgx)

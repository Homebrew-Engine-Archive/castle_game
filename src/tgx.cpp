#include "tgx.h"

NAMESPACE_BEGIN(tgx)

static const char *GetTokenTypeName(TokenType);
static TokenType ExtractTokenType(const Uint8 &token);
static size_t ExtractTokenLength(const Uint8 &token);
static int ReadHeader(SDL_RWops *src, Header *header);

Surface LoadStandaloneImage(SDL_RWops *src)
{
    Header header;
    if(ReadHeader(src, &header)) {
        SDL_Log("Can't read header");
        return Surface();
    }
    
    Uint32 rmask = TGX_RGB16_RMASK;
    Uint32 gmask = TGX_RGB16_GMASK;
    Uint32 bmask = TGX_RGB16_BMASK;
    Uint32 amask = TGX_RGB16_AMASK;
    Uint32 depth = 16;
    Uint32 width = header.width;
    Uint32 height = header.height;
    
    Surface surface = SDL_CreateRGBSurface(
        0, width, height, depth,
        rmask, gmask, bmask, amask);
    
    if(surface.Null()) {
        SDL_Log("Can't allocate surface");
        return Surface();
    }
    
    Sint64 size = ReadableBytes(src);
    if(DecodeTGX(src, size, surface)) {
        SDL_Log("LoadSurface failed");
        return Surface();
    }

    return surface;
}

static int ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < 2 * sizeof(Uint32)) {
        SDL_Log("Source overrun");
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
        SDL_Log("Source buffer overrun");
        return -1;
    }

    if(size < TILE_BYTES) {
        SDL_Log("Given size too small");
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

        if(SDL_RWread(src, dst + offset * bytesPerPixel, bytesPerPixel, length) != length) {
            SDL_Log("Source buffer overrun");
            return -1;
        }
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

int DecodeTGX(SDL_RWops *src, Sint64 size, Surface &surface)
{
    SurfaceLocker lock(surface);

    int bytesPerPixel = surface->format->BytesPerPixel;
    int pitchBytes = surface->pitch;
    int dstBytes = pitchBytes * surface->h;
    Uint8 *dst = reinterpret_cast<Uint8*>(surface->pixels);
    Uint8 *dstBegin = dst;
    Uint8 *dstEnd = dst + dstBytes;
    
    // First position in src which we won't read unless overrun will happen
    Sint64 npos = SDL_RWtell(src) + size;
    
    // src might be overran
    bool overrun = false;

    // surface pixels might be overflowed
    bool overflow = false;

    while(SDL_RWtell(src) < npos) {
        Uint8 token = SDL_ReadU8(src);
        TokenType type = ExtractTokenType(token);
        size_t length = ExtractTokenLength(token);

        switch(type) {
        case TokenType::LineFeed:
            {
                if(length != 1) {
                    SDL_Log("LineFeed token length should be 1");
                    return -1;
                }
                dst += pitchBytes - (dst - dstBegin) % pitchBytes;
                if(dst > dstEnd)
                    overflow = true;
            }
            break;
            
        case TokenType::Transparent:
            {
                dst += length * bytesPerPixel;
                if(dst > dstEnd)
                    overflow = true;
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
                SDL_Log("Undefined token");
                return -1;
            }
            break;
        }

        if(overflow) {
            SDL_Log("Destination buffer overflow");
            return -1;
        }
        if(overrun) {
            SDL_Log("Source buffer overrun");
            return -1;
        }
    }
    
    return 0;
}

NAMESPACE_END(tgx)

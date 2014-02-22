#include "tgx.h"

NAMESPACE_BEGIN(tgx)

static const char *GetTokenTypeName(TokenType);
static TokenType ExtractTokenType(const Uint8 &token);
static size_t ExtractTokenLength(const Uint8 &token);
static int ReadHeader(SDL_RWops *src, Header *header);

template<class Pixel>
int DecodeTGX(SDL_RWops *src, Pixel *dst, size_t pitchBytes, size_t dstBytes)
{
    const Uint8 *dstFirst = reinterpret_cast<Uint8*>(dst);

    // It's going to check buffer overflow (`dst' buffer of `dstBytes' size)
    size_t bytesWritten = 0;

    while(SDL_RWtell(src) < SDL_RWsize(src)) {
        Uint8 token = SDL_ReadU8(src);
        TokenType type = ExtractTokenType(token);
        size_t length = ExtractTokenLength(token);

        switch(type) {
        case TokenType::Stream:
            {
                bytesWritten += length * sizeof(Pixel);
                if(bytesWritten <= dstBytes) {
                    SDL_RWread(src, dst, sizeof(Pixel), length);
                    dst += length;
                }
            }
            break;

        case TokenType::Transparent:
            {
                bytesWritten += length * sizeof(Pixel);
                if(bytesWritten <= dstBytes) {
                    dst += length;
                }
            }
            break;

        case TokenType::Repeat:
            {
                bytesWritten += length * sizeof(Pixel);
                if(bytesWritten <= dstBytes) {
                    Pixel p;
                    SDL_RWread(src, &p, sizeof(Pixel), 1);
                    std::fill(dst, dst + length, p);
                    dst += length;
                }
            }
            break;

        case TokenType::Newline:
            {
                if(length != 1) {
                    SDL_Log("Newline token length should be 1");
                    return -1;
                }
                size_t x = ((Uint8*)dst - dstFirst) % pitchBytes;
                if(x != 0) {
                    size_t skipBytes = pitchBytes - x - 1;
                    bytesWritten += skipBytes;
                    dst = (Pixel*)((Uint8*)dst + skipBytes);
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

        if(bytesWritten > dstBytes) {
            SDL_Log("Buffer overflow, so sad");
            return -1;
        }
    }
    
    return 0;
}

int DecodeUncompressed(SDL_RWops *src, Uint16 *dst, size_t pitchBytes, size_t widthPixels)
{
    size_t widthBytes = widthPixels * sizeof(Uint16);
    while(SDL_RWtell(src) + widthBytes <= SDL_RWsize(src)) {
        SDL_RWread(src, dst, sizeof(Uint16), widthPixels);
        dst = (Uint16*)((Uint8*)dst + pitchBytes);
    }
    
    return 0;
}

int DecodeTile(SDL_RWops *src, Uint16 *dst, size_t pitchBytes)
{
    for(size_t y = 0; y < TILE_RHOMBUS_HEIGHT; ++y) {
        size_t width = TILE_PIXELS_PER_ROW[y];
        size_t offset = (TILE_RHOMBUS_WIDTH - width) / 2;

        SDL_RWread(src, dst + offset, sizeof(Uint16), width);
        dst = (Uint16*)((Uint8*)dst + pitchBytes);
    }
    
    return 0;
}

static int ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < sizeof(Header))
        return -1;
    
    hdr->width = SDL_ReadLE32(src);
    hdr->height = SDL_ReadLE32(src);
    return 0;
}    

struct Uncompressed
{
    static int Decode(SDL_RWops *src, Surface &surface) {
        Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
        const size_t pitch = surface->pitch;
        const size_t width = surface->w;
        return DecodeUncompressed(src, bits, pitch, width);
    }
};

struct Tile
{
    static int Decode(SDL_RWops *src, Surface &surface) {
        Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
        const size_t pitch = surface->pitch;
        return DecodeTile(src, bits, pitch);
    }
};

struct TGX8
{
    static int Decode(SDL_RWops *src, Surface &surface) {
        Uint8 *bits = reinterpret_cast<Uint8*>(surface->pixels);
        const size_t pitch = surface->pitch;
        const size_t pixelBytes = surface->h * pitch;
        return DecodeTGX<Uint8>(src, bits, pitch, pixelBytes);
    }
};

struct TGX16
{
    static int Decode(SDL_RWops *src, Surface &surface) {
        Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
        const size_t pitch = surface->pitch;
        const size_t pixelBytes = surface->h * pitch;
        return DecodeTGX<Uint16>(src, bits, pitch, pixelBytes);
    }
};

template<class Compression>
int LoadSurface(SDL_RWops *src, Sint64 bytes, Surface &surface)
{
    if(bytes > 0) {
        std::vector<Uint8> buffer(bytes);
        SDL_RWread(src, &buffer[0], sizeof(Uint8), bytes);

        auto locked = SDL_MUSTLOCK(surface);
        if(locked != 0)
            SDL_LockSurface(surface);

        SDL_RWops *buffered = SDL_RWFromConstMem(&buffer[0], bytes);
        auto err = Compression::Decode(buffered, surface);
        SDL_RWclose(buffered);

        if(locked != 0)
            SDL_UnlockSurface(surface);
        return err;
    }
    
    return -1;
}

int LoadTGX16Surface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    return LoadSurface<TGX16>(src, size, surface);
}

int LoadTGX8Surface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    return LoadSurface<TGX8>(src, size, surface);
}

int LoadBitmapSurface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    return LoadSurface<Uncompressed>(src, size, surface);
}

int LoadTileSurface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    return LoadSurface<Tile>(src, size, surface);
}

Surface LoadTGX(SDL_RWops *src)
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
    
    Surface surface(
        width, height, depth,
        rmask, gmask, bmask, amask);

    LoadSurface<TGX16>(src, SDL_RWsize(src), surface);

    return surface;
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
    switch(type) {
    case TokenType::Transparent: return "Transparent";
    case TokenType::Stream: return "Stream";
    case TokenType::Newline: return "Newline";
    case TokenType::Repeat: return "Repeat";
    default:
        return "Unknown";
    }
}

NAMESPACE_END(tgx)

#include "tgx.h"

NAMESPACE_BEGIN(tgx)

static const char *GetTokenTypeName(TokenType);
static TokenType ExtractTokenType(const Token &token);
static size_t ExtractTokenLength(const Token &token);

template<class P>
bool ReadPixelArray(SDL_RWops *src, P *bits, size_t count);

// Uint16 spec
template<>
bool ReadPixelArray(SDL_RWops *src, Uint16 *bits, size_t count)
{
    size_t num = SDL_RWread(src, bits, count, sizeof(Uint16));

    if(num < count)
        return false;
    
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint16 *p = bits; p != bits + count; ++p)
            *p = SDL_Swap16(*p);
    }

    return true;
}

// Uint8 spec
template<>
bool ReadPixelArray(SDL_RWops *src, Uint8 *bits, size_t count)
{
    size_t num = SDL_RWread(src, bits, count, sizeof(Uint8));
    return (num == count);
}

template<class Pixel>
void ReadTGX(SDL_RWops *src, Sint64 size, int pitch, size_t bufferSize, Pixel *bits)
{
    Pixel *start = bits;
    Pixel *end = start + bufferSize;
    
    Sint64 origin = SDL_RWtell(src);
    if(origin < 0)
        throw TGXError("file not seekable");

    if(ReadableBytes(src) < size)
        throw TGXError("eof while ReadTGX");

    while(SDL_RWtell(src) < origin + size) {
        Token token;
        SDL_RWread(src, &token, 1, sizeof(Token));

        size_t length = ExtractTokenLength(token);
        TokenType type = ExtractTokenType(token);

        if(type != TokenType::Newline) {
            if(bits + length > end) {
                SDL_LogDebug(
                    SDL_LOG_PRIORITY_ERROR,
                    "at %08x buffer overflow: pitch=%d, bs=%d, bpp=%d, size=%d, TT=%s, TL=%d",
                    SDL_RWtell(src), pitch, bufferSize, sizeof(Pixel), size, length,
                    GetTokenTypeName(type));
                throw TGXError("buffer overflow");
            }
        }
        
        switch(type) {
        case TokenType::Stream:
            {
                ReadPixelArray<Pixel>(src, bits, length);
                bits += length;
                break;
            }            
        case TokenType::Repeat:
            {
                Pixel pixel;
                ReadPixelArray<Pixel>(src, &pixel, 1);
                std::fill(bits, bits + length, pixel);
                bits += length;
                break;
            }
        case TokenType::Transparent:
            {
                bits += length;
                break;
            }
        case TokenType::Newline:
            {
                // Most probably there was an error, throw just in case
                if(length != 1) {
                    SDL_LogDebug(SDL_LOG_PRIORITY_ERROR,
                                 "at %08x newline token length is equal to %d",
                                 SDL_RWtell(src),
                                 static_cast<Uint32>(length));
                    throw TGXError("wrong token length");
                }
                
                // This trick advances `bits` to newline position
                // e.i. position that is multiple by pitch
                size_t x = ((Uint8*)bits - (Uint8*)start) % pitch;
                if(x != 0)
                    bits = (Pixel*)((Uint8*)bits + pitch - x - 1);
                break;
            }
        default:
            {
                SDL_LogDebug(SDL_LOG_PRIORITY_ERROR,
                             "at %08x unknown token %d",
                             SDL_RWtell(src), token);
                throw TGXError("wrong token type");
            }
        }
    }
}

void ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < sizeof(Header))
        throw TGXError("EOF while ReadHeader");
    
    hdr->width = SDL_ReadLE32(src);
    hdr->height = SDL_ReadLE32(src);
}    

void ReadTile(SDL_RWops *src, int pitch, Uint16 *bits)
{
    if(ReadableBytes(src) < TILE_BYTES)
        throw TGXError("EOF while ReadTile");
    
    for(size_t row = 0; row < TILE_RHOMBUS_HEIGHT; ++row) {
        size_t pixelsPerRow = TILE_PIXELS_PER_ROW[row];
        size_t offset = (TILE_RHOMBUS_WIDTH - pixelsPerRow) / 2;
        ReadPixelArray<Uint16>(src, bits + offset, pixelsPerRow);
        bits = (Uint16*)((Uint8*)bits + pitch);
    }
}

void ReadUncompressed(SDL_RWops *src, Sint64 size, int pitch, int width, size_t bufferSize, Uint16 *bits)
{
    Sint64 origin = SDL_RWtell(src);

    if(ReadableBytes(src) < size)
        throw TGXError("eof while reading bitmap");

    if(size > bufferSize * sizeof(Uint16))
        throw TGXError("buffer overflow");
    
    while(SDL_RWtell(src) < origin + size) {
        ReadPixelArray<Uint16>(src, bits, width);
        bits = (Uint16*)((Uint8*)bits + pitch);
    }
}

void LoadTGX16Surface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    auto locked = SDL_MUSTLOCK(surface);
    if(locked != 0)
        SDL_LockSurface(surface);
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    const int pitch = surface->pitch;
    const size_t bufferSize = pitch * surface->h;
    ReadTGX<Uint16>(src, size, pitch, bufferSize, bits);
    if(locked != 0)
        SDL_UnlockSurface(surface);
}

void LoadTGX8Surface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    auto locked = SDL_MUSTLOCK(surface);
    if(locked != 0)
        SDL_LockSurface(surface);
    Uint8 *bits = reinterpret_cast<Uint8*>(surface->pixels);
    const int pitch = surface->pitch;
    const size_t bufferSize = pitch * surface->h;
    ReadTGX<Uint8>(src, size, pitch, bufferSize, bits);
    if(locked != 0)
        SDL_UnlockSurface(surface);
}

void LoadBitmapSurface(SDL_RWops *src, Sint64 size, Surface &surface)
{
    auto locked = SDL_MUSTLOCK(surface);
    if(locked)
        SDL_LockSurface(surface);
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    const int pitch = surface->pitch;
    const int width = surface->w;
    const size_t bufferSize = pitch * surface->h;
    ReadUncompressed(src, size, width, pitch, bufferSize, bits);
    if(locked != 0)
        SDL_UnlockSurface(surface);
}

void LoadTileSurface(SDL_RWops *src/*, Sint64 size = TILE_BYTES */, Surface &surface)
{
    auto locked = SDL_MUSTLOCK(surface);
    if(locked != 0)
        SDL_LockSurface(surface);
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    ReadTile(src, surface->pitch, bits);
    if(locked != 0)
        SDL_UnlockSurface(surface);
}

Surface LoadTGX(SDL_RWops *src)
{
    Header header;
    ReadHeader(src, &header);

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

    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    const Uint32 size = ReadableBytes(src);
    const size_t bufferSize = width * height;
    ReadTGX<Uint16>(src, size, width, bufferSize, bits);

    return surface;
}

static size_t ExtractTokenLength(const Token &token)
{
    // Lower 5 bits represent range between [1..32]
    return (token & 0x1f) + 1;
}

static TokenType ExtractTokenType(const Token &token)
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

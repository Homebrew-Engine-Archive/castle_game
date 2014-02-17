#include "tgx.h"

NAMESPACE_BEGIN(tgx)

static int PopCount(Uint32 n);
static bool CheckTokenType(const TGXToken &token);
static bool CheckTokenLength(const TGXToken &token);
static void ReadTGXToken(SDL_RWops *src, TGXToken *token);
static bool CheckTGXSize(Uint32 width, Uint32 height);

template<class P>
void ReadPixelArray(SDL_RWops *src, P *bits, Uint32 count);

// Uint16 spec
template<>
void ReadPixelArray(SDL_RWops *src, Uint16 *bits, Uint32 count)
{
    SDL_RWread(src, bits, count, sizeof(Uint16));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint16 *p = bits; p != bits + count; ++p)
            *p = SDL_Swap16(*p);
    }
}

// Uint8 spec
template<>
void ReadPixelArray(SDL_RWops *src, Uint8 *bits, Uint32 count)
{
    SDL_RWread(src, bits, count, sizeof(Uint8));
}

static void ReadTGXToken(SDL_RWops *src, TGXToken *token)
{
    SDL_RWread(src, token, 1, sizeof(TGXToken));
}

static bool CheckTokenType(const TGXToken &token)
{
    // There should be only single non-zero bit.
    return (PopCount(token >> 5) <= 1);
}

// Hamming weight, count non-zero bits in the number
static int PopCount(Uint32 n)
{
#ifdef __builtin_popcount
    return __builtin_popcount(n);
#else
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
#endif
}

static bool CheckTokenLength(const TGXToken &token)
{
    // Length for Newline token should be always equal to 1
    return (GetTokenType(token) != TokenType::Newline)
        || (GetTokenLength(token) == 1);
}

static bool CheckTGXSize(Uint32 width, Uint32 height)
{
    return (width <= MAX_TGX_WIDTH) && (height <= MAX_TGX_HEIGHT);
}

template<class P>
static void ReadTGX(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, P *bits)
{   
    P *start = bits;
    P *end = bits + (width * height);

    if(!CheckTGXSize(width, height))
        throw TGXError("TGX size exceeded");
    
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    if(origin < 0)
        throw TGXError("can't seek anymore");
    
    while(SDL_RWseek(src, 0, RW_SEEK_CUR) < origin + size) {
        TGXToken token;
        ReadTGXToken(src, &token);

        Uint32 tokenLength = GetTokenLength(token);
        TokenType tokenType = GetTokenType(token);
        
        if(!CheckTokenType(token)) {
            Sint64 pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
            SDL_Log("%08x: Wrong token type %d",
                    static_cast<Uint32>(pos),
                    static_cast<Uint32>(tokenType));
            throw TGXError("Wrong token type");
        }
        
        if(!CheckTokenLength(token)) {
            Sint64 pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
            SDL_Log("%08x: Wrong token length %d",
                    static_cast<Uint32>(pos),
                    static_cast<Uint32>(tokenLength));
            throw TGXError("Wrong token length");
        }

        if(tokenType != TokenType::Newline) {
            if(bits + tokenLength > end)
                throw TGXError("Buffer overflow");
        }
        
        switch(tokenType) {
        case TokenType::Stream:
            {
                ReadPixelArray<P>(src, bits, tokenLength);
                bits += tokenLength;
                break;
            }            
        case TokenType::Repeat:
            {
                P pixel;
                ReadPixelArray<P>(src, &pixel, 1);
                std::fill(bits, bits + tokenLength, pixel);
                bits += tokenLength;
                break;
            }            
        case TokenType::Transparent:
            {
                bits += tokenLength;
                break;
            }
        case TokenType::Newline:
            {
                // This thing advances `bits` to newline position
                // e.i. position times width
                if((bits - start) % width != 0)
                    bits += width - (bits - start) % width - 1;
                break;
            }
        }
    }
}

void ReadTGX16(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, Uint16 *bits)
{
    ReadTGX<Uint16>(src, size, width, height, bits);
}

void ReadTGX8(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, Uint8 *bits)
{
    ReadTGX<Uint8>(src, size, width, height, bits);
}

void ReadTGXHeader(SDL_RWops *src, TGXHeader *hdr)
{
    if(ReadableBytes(src) < sizeof(TGXHeader))
        throw TGXError("EOF while ReadTGXHeader");
    
    hdr->width = SDL_ReadLE32(src);
    hdr->height = SDL_ReadLE32(src);
}    

void ReadBitmap(SDL_RWops *src, Uint32 size, Uint16 *bits)
{
    ReadPixelArray<Uint16>(src, bits, size / sizeof(Uint16));
}

void ReadTile(SDL_RWops *src, Uint16 *bits)
{
    if(ReadableBytes(src) < TILE_BYTES)
        throw TGXError("EOF while ReadTile");
    
    for(size_t row = 0; row < TILE_RHOMBUS_HEIGHT; ++row) {
        size_t offset = (TILE_RHOMBUS_WIDTH - TILE_PIXELS_PER_ROW[row]) / 2;
        ReadPixelArray<Uint16>(src, bits + offset, TILE_PIXELS_PER_ROW[row]);
        bits += TILE_RHOMBUS_WIDTH;
    }
}

Surface LoadTGX(SDL_RWops *src)
{
    TGXHeader header;
    ReadTGXHeader(src, &header);

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
    
    Uint32 size = ReadableBytes(src);
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    ReadTGX<Uint16>(src, size, width, height, bits);

    return surface;
}

NAMESPACE_END(tgx)

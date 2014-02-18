#include "tgx.h"

NAMESPACE_BEGIN(tgx)

static const char *GetTokenTypeName(TokenType t);
static TokenType ExtractTokenType(const Token &token);
static Uint32 ExtractTokenLength(const Token &token);

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

template<class P>
void ReadTGX(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, P *bits)
{   
    P *start = bits;
    P *end = bits + (width * height);
    
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    if(origin < 0)
        throw TGXError("File not seekable");

    if(ReadableBytes(src) < size)
        throw TGXError("EOF while ReadTGX");
    
    while(SDL_RWseek(src, 0, RW_SEEK_CUR) < origin + size) {
        Token token;
        SDL_RWread(src, &token, 1, sizeof(Token));

        Uint32 length = ExtractTokenLength(token);
        TokenType type = ExtractTokenType(token);

        if(type != TokenType::Newline) {
            if(bits + length > end) {
                SDL_LogDebug(SDL_LOG_PRIORITY_ERROR,
                             "at %08x buffer overflow: w=%d, h=%d, bpp=%d, size=%d, TT=%s, TL=%d",
                             width, height, sizeof(P), size,
                             GetTokenTypeName(type), length);
                throw TGXError("Buffer overflow");
            }
        }
        
        switch(type) {
        case TokenType::Stream:
            {
                ReadPixelArray<P>(src, bits, length);
                bits += length;
                break;
            }            
        case TokenType::Repeat:
            {
                P pixel;
                ReadPixelArray<P>(src, &pixel, 1);
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
                    Sint64 pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
                    SDL_LogDebug(SDL_LOG_PRIORITY_WARN,
                                 "at %08x wrong token length %d",
                                 static_cast<Uint32>(pos),
                                 static_cast<Uint32>(length));
                    throw TGXError("Wrong token length");
                }
                
                // This thing advances `bits` to newline position
                // e.i. position times width
                if((bits - start) % width != 0)
                    bits += width - (bits - start) % width - 1;
                break;
            }
        case TokenType::Unknown:
            {
                Sint64 pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
                SDL_LogDebug(SDL_LOG_PRIORITY_WARN,
                             "at %08x unknown token type %d",
                             static_cast<Uint32>(pos),
                             static_cast<Uint32>(type));
                throw TGXError("Wrong token type");
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

void ReadHeader(SDL_RWops *src, Header *hdr)
{
    if(ReadableBytes(src) < sizeof(Header))
        throw TGXError("EOF while ReadHeader");
    
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
        size_t width = TILE_PIXELS_PER_ROW[row];
        size_t pitch = TILE_RHOMBUS_WIDTH;
        size_t offset = (pitch - width) / 2;
        ReadPixelArray<Uint16>(src, bits + offset, width);
        bits += pitch;
    }
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
    
    Uint32 size = ReadableBytes(src);
    Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
    ReadTGX<Uint16>(src, size, width, height, bits);

    return surface;
}

static Uint32 ExtractTokenLength(const Token &token)
{
    // Lower 5 bits represent range between [1..32]
    return (token & 0x1f) + 1;
}

static TokenType ExtractTokenType(const Token &token)
{
    // Higher 3 bits
    switch(token >> 5) {
    case TGX_TOKEN_TYPE_STREAM: return TokenType::Stream;
    case TGX_TOKEN_TYPE_TRANSPARENT: return TokenType::Transparent;
    case TGX_TOKEN_TYPE_REPEAT: return TokenType::Repeat;
    case TGX_TOKEN_TYPE_NEWLINE: return TokenType::Newline;
    default:
        return TokenType::Unknown;
    }
}

static const char * GetTokenTypeName(TokenType t)
{
    switch(t) {
    case TokenType::Transparent: return "Transparent";
    case TokenType::Stream: return "Stream";
    case TokenType::Newline: return "Newline";
    case TokenType::Repeat: return "Repeat";
    default:
        return "Unknown";
    }
}

NAMESPACE_END(tgx)

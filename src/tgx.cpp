#include "tgx.h"

int PopCount(Uint8 n)
{
    int count = 0;
    while(n != 0) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

bool CheckTokenType(const TGXToken &token)
{
    // There should be only single non-zero bit.
    return (PopCount(token >> 5) <= 1);
}

bool CheckTokenLength(const TGXToken &token)
{
    // Length for Newline token should be always equal to 1
    return (GetTokenType(token) != TokenType::Newline)
        || (GetTokenLength(token) == 1);
}

Sint64 GetAvailableBytes(SDL_RWops *src)
{
    return SDL_RWsize(src) - SDL_RWseek(src, 0, RW_SEEK_CUR);
}

bool CheckBytesAvailable(SDL_RWops *src, Sint64 bytes) throw()
{
    return (GetAvailableBytes(src) >= bytes);
}

bool CheckTGXSize(Uint32 width, Uint32 height)
{
    return (width <= MAX_TGX_WIDTH) && (height <= MAX_TGX_HEIGHT);
}

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

    if(!CheckTGXSize(width, height))
        throw FormatError("Image too big");
    
    Sint64 origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    while(SDL_RWseek(src, 0, RW_SEEK_CUR) < origin + size) {
        TGXToken token;
        ReadTGXToken(src, &token);

        Uint32 tokenLength = GetTokenLength(token);
        TokenType tokenType = GetTokenType(token);
        
        if(!CheckTokenType(token)) {
            Uint32 pos = static_cast<Uint32>(SDL_RWseek(src, 0, RW_SEEK_CUR));
            SDL_Log("%08x: Wrong token type %d", pos,
                    static_cast<Uint32>(tokenType));
            throw FormatError("Wrong token type");
        }
        
        if(!CheckTokenLength(token)) {
            Uint32 pos = static_cast<Uint32>(SDL_RWseek(src, 0, RW_SEEK_CUR));
            SDL_Log("%08x: Wrong token length %d", pos,
                    static_cast<Uint32>(tokenLength));
            throw FormatError("Wrong token length");
        }

        if(tokenType != TokenType::Newline) {
            if(bits + tokenLength > end)
                throw FormatError("Buffer overflow");
        }
        
        switch(tokenType) {
        case TokenType::Stream:
            ReadPixelArray<P>(src, bits, tokenLength);
            bits += tokenLength;
            break;
            
        case TokenType::Repeat: {
            P pixel;
            ReadPixelArray<P>(src, &pixel, 1);
            std::fill(bits, bits + tokenLength, pixel);
            bits += tokenLength;
            break;
        }
            
        case TokenType::Transparent:
            bits += tokenLength;
            break;
            
        case TokenType::Newline:
            // This thing advances `bits` to newline position
            // e.i. position times width
            if((bits - start) % width != 0)
                bits += width - (bits - start) % width - 1;
            break;
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
    if(!CheckBytesAvailable(src, sizeof(TGXHeader)))
        throw EOFError("ReadTGXHeader");
    
    hdr->width = SDL_ReadLE32(src);
    hdr->height = SDL_ReadLE32(src);
}    

void ReadTGXToken(SDL_RWops *src, TGXToken *token)
{
    SDL_RWread(src, token, 1, sizeof(TGXToken));
}

void ReadBitmap(SDL_RWops *src, Uint32 size, Uint16 *bits)
{
    ReadPixelArray<Uint16>(src, bits, size / sizeof(Uint16));
}

void ReadTile(SDL_RWops *src, Uint16 *bits)
{
    if(!CheckBytesAvailable(src, TILE_BYTES))
        throw EOFError("ReadTile");
    
    for(size_t row = 0; row < TILE_RHOMBUS_HEIGHT; ++row) {
        size_t offset = (TILE_RHOMBUS_WIDTH - TILE_PIXELS_PER_ROW[row]) / 2;
        ReadPixelArray<Uint16>(src, bits + offset, TILE_PIXELS_PER_ROW[row]);
        bits += TILE_RHOMBUS_WIDTH;
    }
}

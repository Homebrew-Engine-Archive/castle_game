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
void ReadPixelArray(SDL_RWops *src, P *buffer, Uint32 count);

// Uint16 spec
template<>
void ReadPixelArray(SDL_RWops *src, Uint16 *buffer, Uint32 count)
{
    SDL_RWread(src, buffer, count, sizeof(Uint16));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint16 *p = buffer; p != buffer + count; ++p)
            *p = SDL_Swap16(*p);
    }
}

// Uint8 spec
template<>
void ReadPixelArray(SDL_RWops *src, Uint8 *buffer, Uint32 count)
{
    SDL_RWread(src, buffer, count, sizeof(Uint8));
}

template<class P>
void ReadTGX(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, P *pImg)
{
    TGXToken token;
    
    auto pStart = pImg;
    auto pEnd = pStart + width * height;

    if(!CheckTGXSize(width, height))
        throw FormatError("Image too big");
    
    auto origin = SDL_RWseek(src, 0, RW_SEEK_CUR);
    while(SDL_RWseek(src, 0, RW_SEEK_CUR) < origin + size) {
        ReadTGXToken(src, &token);
        
        if(!CheckTokenType(token)) {
            Uint32 pos = static_cast<Uint32>(SDL_RWseek(src, 0, RW_SEEK_CUR));
            SDL_Log("%08x: Wrong TGXToken type %d",
                    pos, token);
            throw FormatError("Wrong token type");
        }
        
        if(!CheckTokenLength(token)) {
            Uint32 pos = static_cast<Uint32>(SDL_RWseek(src, 0, RW_SEEK_CUR));
            SDL_Log("%08x: Wrong TGXToken length %d",
                    pos, token);
            throw FormatError("Wrong token length");
        }

        if(GetTokenType(token) != TokenType::Newline) {
            if(pImg + GetTokenLength(token) > pEnd)
                throw FormatError("Buffer overflow");
        }
        
        switch(GetTokenType(token)) {
        case TokenType::Stream:
            ReadPixelArray<P>(src, pImg, GetTokenLength(token));
            pImg += GetTokenLength(token);
            break;
            
        case TokenType::Repeat: {
            P pixel;
            ReadPixelArray<P>(src, &pixel, 1);
            std::fill(pImg, pImg + GetTokenLength(token), pixel);
            pImg += GetTokenLength(token);
            break;
        }
            
        case TokenType::Transparent:
            pImg += GetTokenLength(token);
            break;
            
        case TokenType::Newline:
            if((pImg - pStart) % width != 0)
                pImg += width - (pImg - pStart) % width - 1;
            break;
        }
    }
}

void ReadTGX16(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, Uint16 *pImg)
{
    ReadTGX<Uint16>(src, size, width, height, pImg);
}

void ReadTGX8(SDL_RWops *src, Uint32 size, Uint32 width, Uint32 height, Uint8 *pImg)
{
    ReadTGX<Uint8>(src, size, width, height, pImg);
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

void ReadBitmap(SDL_RWops *src, Uint32 size, Uint16 *pixels)
{
    ReadPixelArray<Uint16>(src, pixels, size / sizeof(Uint16));
}

void ReadTile(SDL_RWops *src, Uint16 *pixels)
{
    if(!CheckBytesAvailable(src, TILE_BYTES))
        throw EOFError("ReadTile");
    
    for(size_t row = 0; row < TILE_RHOMBUS_HEIGHT; ++row) {
        size_t offset = (TILE_RHOMBUS_WIDTH - TILE_PIXELS_PER_ROW[row]) / 2;
        ReadPixelArray<Uint16>(src, pixels + offset, TILE_PIXELS_PER_ROW[row]);
        pixels += TILE_RHOMBUS_WIDTH;
    }
}

#include "TGX.h"

Sint64 ReadTGXHeader(SDL_RWops *src, TGXHeader *header)
{
    header->width = SDL_ReadLE32(src);
    header->height = SDL_ReadLE32(src);
    return 0;
}

Sint64 ReadTGXToken(SDL_RWops *src, TGXToken *token)
{
    if(src->read(src, &token->rawdata, 1, 1) < 0)
        return -1;
    
    return 0;
}

int PopCount(Uint8 n)
{
    int count = 0;
    while(n != 0) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

Sint64 CheckSourceSize(SDL_RWops *src, Sint64 bytes)
{
    if(src->size(src) < bytes) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Need to get %d bytes more, but there is only %d.\n", bytes, src->size(src));
        return -1;
    }

    return 0;
}

Sint64 ReadTGXPixelsNoPalette(SDL_RWops *src, Uint16 width, Uint16 height, Uint16 *pixels)
{
    TGXToken token;

    // Expected start and end of pixel buffer
    Uint16 *pBegin = pixels;
    Uint16 *pEnd = pixels + width * height;

    while(pixels != pEnd) {
        if(0 != ReadTGXToken(src, &token)) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ReadTGXToken failed: %s\n",
                            SDL_GetError());
            return -1;
        }
        TokenType tokenType = GetTokenType(token);
        if(PopCount(token.rawdata >> 5) > 1) {
            SDL_Log("Wrong token format (t: %d, n: %d, r: %d)\n",
                    token.data.transparent, token.data.newline, token.data.repeat);
        }
        Uint32 tokenLength = GetTokenLength(token);
        if((tokenLength < 1) || (tokenLength > 32)) {
            SDL_Log("Wrong token length: %d\n", tokenLength);
        }        

        if((tokenType != TokenType_Newline) && (pixels + tokenLength) > pEnd) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Buffer overflow.");
            return -1;
        }
        SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "TokenType: %s, Length: %d\n", GetTokenTypeName(tokenType),
                tokenLength);
        
        switch(tokenType) {
        case TokenType_Stream: 
            if(0 != CheckSourceSize(src, sizeof(Uint16) * tokenLength)) {
                return -1;
            }
            for(Uint16 n = 0; n < tokenLength; ++n) {
                pixels[n] = SDL_ReadLE16(src);
            }
            pixels += tokenLength;
            break;
        case TokenType_Repeat: {
            if(0 != CheckSourceSize(src, sizeof(Uint16))) {
                return -1;
            }
            Uint16 pixelData = SDL_ReadLE16(src);
            for(Uint16 n = 0; n < tokenLength; ++n)
                pixels[n] = pixelData;
            pixels += tokenLength;
            break;
        }
        case TokenType_Newline:
            if(tokenLength != 1) {
                SDL_Log("Newline token length should be 1, but %d found.\n",
                    tokenLength);
            }
            while((pixels - pBegin) % width != 0)
                ++pixels;
            break;
        case TokenType_Transparent:
            pixels += tokenLength;
            break;
        }
    }

    return 0;
}

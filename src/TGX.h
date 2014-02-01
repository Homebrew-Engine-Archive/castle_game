#ifndef TGX_H_
#define TGX_H_

#include <algorithm>
#include <SDL2/SDL.h>

const Uint32 MAX_TGX_WIDTH = 4096;
const Uint32 MAX_TGX_HEIGHT = 4096;

struct TGXHeader
{
    Uint32 width;
    Uint32 height;
};

union TGXToken
{
    Uint8 rawdata;
    struct {
        Uint32 length : 5;
        Uint32 transparent : 1;
        Uint32 repeat : 1;
        Uint32 newline : 1;
    } data;
};

enum TokenType {TokenType_Stream,
                TokenType_Transparent,
                TokenType_Newline,
                TokenType_Repeat};

constexpr TokenType GetTokenType(const TGXToken &token)
{
    return token.data.newline ? TokenType_Newline
        : token.data.repeat ? TokenType_Repeat
        : token.data.transparent ? TokenType_Transparent
        : TokenType_Stream;
}

constexpr Uint32 GetTokenLength(const TGXToken &token)
{
    return token.data.length + 1;
}

constexpr const char * GetTokenTypeName(TokenType t)
{
    return t == TokenType_Transparent ? "Transparent"
        : t == TokenType_Stream ? "Stream"
        : t == TokenType_Newline ? "Newline"
        : "Repeat";
}

Sint64 ReadTGXHeader(SDL_RWops *src, TGXHeader *header);
Sint64 ReadTGXToken(SDL_RWops *src, TGXToken *token);
Sint64 ReadTGXPixelsNoPalette(SDL_RWops *src, Uint16 width, Uint16 height, Uint16 *pixels);

#endif

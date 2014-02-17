#include "rw.h"

void RWCloseDeleter::operator()(SDL_RWops *src) const
{
    SDL_RWclose(src);
}

Uint32 ReadableBytes(SDL_RWops *src)
{
    Sint64 size = SDL_RWsize(src);
    Sint64 pos = SDL_RWseek(src, 0, RW_SEEK_CUR);
    return ((size < 0) || (pos < 0) || (size < pos))
        ? 0
        : size - pos;
}

void ReadInt32Array(SDL_RWops *src, Uint32 *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(Uint32));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint32 *ptr = buffer; ptr != buffer + num; ++ptr)
            *ptr = SDL_Swap32(*ptr);
    }
}

void ReadInt16Array(SDL_RWops *src, Uint16 *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(Uint16));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint16 *ptr = buffer; ptr != buffer + num; ++ptr)
            *ptr = SDL_Swap16(*ptr);
    }
}

void ReadInt8Array(SDL_RWops *src, Uint8 *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(Uint8));

}

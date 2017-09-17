#ifndef ENDIANNESS_H_
#define ENDIANNESS_H_

#include <iostream>
#include <SDL_endian.h>

namespace core
{
    template<class T> T SwapBig(T big);
    template<class T> T SwapLittle(T little);
    
    template<>
    inline uint64_t SwapLittle(uint64_t little)
    {
        return SDL_SwapLE64(little);
    }
    
    template<>
    inline uint32_t SwapLittle(uint32_t little)
    {
        return SDL_SwapLE32(little);
    }
    
    template<>
    inline uint16_t SwapLittle(uint16_t little)
    {
        return SDL_SwapLE16(little);
    }

    template<>
    inline uint8_t SwapLittle(uint8_t little)
    {
        return little;
    }

    template<>
    inline int64_t SwapLittle(int64_t little)
    {
        return SDL_SwapLE64(little);
    }

    template<>
    inline int32_t SwapLittle(int32_t little)
    {
        return SDL_SwapLE32(little);
    }

    template<>
    inline int16_t SwapLittle(int16_t little)
    {
        return SDL_SwapLE16(little);
    }

    template<>
    inline int8_t SwapLittle(int8_t little)
    {
        return little;
    }

    template<>
    inline int64_t SwapBig(int64_t big)
    {
        return SDL_SwapBE64(big);
    }

    template<>
    inline int32_t SwapBig(int32_t big)
    {
        return SDL_SwapBE32(big);
    }

    template<>
    inline int16_t SwapBig(int16_t big)
    {
        return SDL_SwapBE16(big);
    }

    template<>
    inline int8_t SwapBig(int8_t big)
    {
        return big;
    }
    
    template<>
    inline uint64_t SwapBig(uint64_t big)
    {
        return SDL_SwapBE64(big);
    }
    
    template<>
    inline uint32_t SwapBig(uint32_t big)
    {
        return SDL_SwapBE32(big);
    }

    template<>
    inline uint16_t SwapBig(uint16_t big)
    {
        return SDL_SwapBE16(big);
    }

    template<>
    inline uint8_t SwapBig(uint8_t big)
    {
        return big;
    }    
}

#endif  // ENDIANNESS_H_

#ifndef ENDIANNESS_H_
#define ENDIANNESS_H_

#include <type_traits>
#include <iosfwd>
#include <SDL_endian.h>

namespace Endian
{

    template<class T> T SwapLittle(T x)
    {
        static_assert(std::is_integral<T>::value, "Swap little not specialized for this type");
    }
    
    template<> inline uint64_t SwapLittle(uint64_t x)
    {
        return SDL_SwapLE64(x);
    }
    
    template<> inline uint32_t SwapLittle(uint32_t x)
    {
        return SDL_SwapLE32(x);
    }

    template<> inline uint16_t SwapLittle(uint16_t x)
    {
        return SDL_SwapLE16(x);
    }

    template<> inline uint8_t SwapLittle(uint8_t x)
    {
        return x;
    }

    template<> inline int64_t SwapLittle(int64_t x)
    {
        return SDL_SwapLE64(x);
    }

    template<> inline int32_t SwapLittle(int32_t x)
    {
        return SDL_SwapLE32(x);
    }

    template<> inline int16_t SwapLittle(int16_t x)
    {
        return SDL_SwapLE16(x);
    }

    template<> inline int8_t SwapLittle(int8_t x)
    {
        return x;
    }
    
    template<class T> T ReadLittle(std::istream &in)
    {
        T x = 0;
        in.read(reinterpret_cast<char*>(&x), sizeof(T));
        return SwapLittle<T>(x);
    }
    
} // namespace Endian

#endif

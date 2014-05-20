#ifndef IOHELPERS_H_
#define IOHELPERS_H_

#include <game/endianness.h>
#include <iostream>

namespace io
{
    template<class T>
    inline std::istream& ReadLittle(std::istream &in, T &little)
    {
        in.read(reinterpret_cast<char*>(&little), sizeof(T));
        little = Endian::SwapLittle(little);
        return in;
    }

    template<class T>
    inline std::istream& ReadBig(std::istream &in, T &big)
    {
        in.read(reinterpret_cast<char*>(&big), sizeof(T));
        big = Endian::SwapBig(big);
        return in;
    }

    template<class T>
    inline std::ostream& WriteLittle(std::ostream &out, T little)
    {
        const T swapped = Endian::SwapLittle(little);
        out.write(reinterpret_cast<char const*>(&swapped), sizeof(T));
        return out;
    }

    template<class T>
    inline std::ostream& WriteBig(std::ostream &out, T big)
    {
        const T swapped = Endian::SwapBig(big);
        out.write(reinterpret_cast<char const*>(&swapped), sizeof(T));
        return out;
    }
}

#endif // IOHELPERS_H_

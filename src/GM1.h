#ifndef GM1_H_
#define GM1_H_

#include <cstdint>

const int PALETTE_MAX_SETS = 10;
const int PALETTE_MAX_COLORS = 256;

struct GM1Header
{
    Uint32 u0[3];
    Uint32 entryCount;
    Uint32 u1;
    Uint32 dataType;
    Uint32 u2[14];
    Uint32 dataSize;
    Uint16 palette[PALETTE_MAX_SETS][PALETTE_MAX_COLORS];
};

enum class GM1DataType : std::uint32_t {
    Interface = 1,
    Animations = 2,
    Buildings = 3,
    Font = 4,
    Walls = 5,
    Grass = 6,
    Other = 7
};

Sint64 ReadGM1Header()

#endif

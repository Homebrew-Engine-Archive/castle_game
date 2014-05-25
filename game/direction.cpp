#include "direction.h"

#include <cmath>

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <limits>

#include <game/point.h>

namespace
{
    void test()
    {
        constexpr Direction E   = Direction::East;
        constexpr Direction NE  = Direction::NorthEast;
        constexpr Direction N   = Direction::North;
        constexpr Direction NW  = Direction::NorthWest;
        constexpr Direction W   = Direction::West;
        constexpr Direction SW  = Direction::SouthWest;
        constexpr Direction S   = Direction::South;
        constexpr Direction SE  = Direction::SouthEast;
        
        static_assert(GetOppositeDirection(E) == W, "E isn't opposite to W");
        static_assert(GetOppositeDirection(NE) == SW, "NE isn't opposite to");
        static_assert(GetOppositeDirection(N) == S, "N isn't opposite to S");
        static_assert(GetOppositeDirection(NW) == SE, "NW isn't opposite to");
        static_assert(GetOppositeDirection(W) == E, "W isn't opposite to E");
        static_assert(GetOppositeDirection(SW) == NE, "SW isn't opposite to");
        static_assert(GetOppositeDirection(S) == N, "S isn't opposite to N");
        static_assert(GetOppositeDirection(SE) == NW, "SE isn't opposite to NW");
    }
}

Direction RadiansToDirection(double angle)
{
    // Avoid negative angles
    double alpha = fmod(2 * M_PI + angle, 2 * M_PI);

    // Rotate by half of the direction's angle
    double beta = (alpha - 1.0f / (2 * MaxDirCount));

    // scale down to range [0, 1)
    double gamma = beta / (2 * M_PI);

    // and now avoid cyclic directions
    int d = core::Mod<int>(gamma * MaxDirCount, MaxDirCount);
    
    return static_cast<Direction>(d);
}

double DirectionToRadians(Direction dir)
{
    switch(dir) {
    case Direction::East: return 0.0f;
    case Direction::NorthEast: return M_PI / 4.0f;
    case Direction::North: return M_PI / 2.0f;
    case Direction::NorthWest: return 3.0f * M_PI / 4.0f;
    case Direction::West: return M_PI;
    case Direction::SouthWest: return M_PI + M_PI / 4.0f;
    case Direction::South: return 3.0f * M_PI / 2.0f;
    case Direction::SouthEast: return 3.0f * M_PI / 4.0f + M_PI;
    }

    throw std::runtime_error("invalid direction");
}

Direction PointsDirection(const Point &lhs, const Point &rhs)
{
    return RadiansToDirection(atan2(lhs.x - rhs.x, lhs.y - rhs.y));
}

Direction ClosestDirection(const DirectionSet &set, const Direction &dir)
{
    Direction closest = dir;
    int bestDist = std::numeric_limits<int>::max();
    for(const Direction &d : set) {
        int dist = MinRotates(d, dir);
        if(bestDist > dist) {
            closest = d;
            bestDist = dist;
        }
    }
    return closest;
}

std::ostream &operator<<(std::ostream &out, const Direction &dir)
{
#define PRINT_IN_CASE(value) case (value): out << #value
    switch(dir) {
        PRINT_IN_CASE(Direction::East);
        PRINT_IN_CASE(Direction::NorthEast);
        PRINT_IN_CASE(Direction::North);
        PRINT_IN_CASE(Direction::NorthWest);
        PRINT_IN_CASE(Direction::West);
        PRINT_IN_CASE(Direction::SouthWest);
        PRINT_IN_CASE(Direction::South);
        PRINT_IN_CASE(Direction::SouthEast);
    default:
        out << "Unknown direction";
    }
#undef PRINT_IN_CASE

    return out;
}

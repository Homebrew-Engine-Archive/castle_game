#include "direction.h"

#include <cmath>

#include <exception>
#include <algorithm>
#include <iostream>
#include <limits>

#include <core/point.h>

namespace
{
    struct invalid_direction : public std::exception
    {
        const char* what() const throw() {
            return "invalid direction";
        }
    };
    
    void test()
    {
        using namespace core;
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

namespace core
{
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
    
        return DirFromNum(d);
    }

    double DirectionToRadians(Direction dir)
    {
        return core::Mod((DirToNum(dir) - DirToNum(Direction::East)), MaxDirCount) * 2 * M_PI / MaxDirCount;
    }

    Direction PointsDirection(const Point &lhs, const Point &rhs)
    {
        return RadiansToDirection(atan2(lhs.X() - rhs.X(), lhs.Y() - rhs.Y()));
    }

    std::ostream &operator<<(std::ostream &out, const Direction &dir)
    {
#define CASE(value) case (value): out << #value
        switch(dir) {
            CASE(Direction::East);
            CASE(Direction::NorthEast);
            CASE(Direction::North);
            CASE(Direction::NorthWest);
            CASE(Direction::West);
            CASE(Direction::SouthWest);
            CASE(Direction::South);
            CASE(Direction::SouthEast);
        default:
            throw invalid_direction();
        }
#undef PRINT_IN_CASE

        return out;
    }
}

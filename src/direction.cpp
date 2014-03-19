#include "direction.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace
{

    const int DIR_COUNT = 8;

    Direction Rotated(const Direction &dir, int times)
    {
        return Direction(
            (static_cast<int>(dir) + times + DIR_COUNT) % DIR_COUNT);
    }
    
}

Direction RotatedLeft(const Direction &dir, int times)
{
    return Rotated(dir, -times);
}

Direction RotatedRight(const Direction &dir, int times)
{
    return Rotated(dir, times);
}

int LeftAngle(const Direction &lhs, const Direction &rhs)
{
    return (static_cast<int>(lhs) - static_cast<int>(rhs) + DIR_COUNT) % DIR_COUNT;
}

int RightAngle(const Direction &lhs, const Direction &rhs)
{
    return LeftAngle(rhs, lhs);
}

int MinAngle(const Direction &lhs, const Direction &rhs)
{
    return std::min(LeftAngle(lhs, rhs),
                    RightAngle(lhs, rhs));
}

Direction RadiansToDirection(double angle)
{
    const static double PI = 3.141593;

    // Avoid negative angles
    double alpha = fmod(2 * PI + angle, 2 * PI);

    // Rotate by half of the direction's angle
    double beta = (alpha - 1.0f / (2 * DIR_COUNT));

    // scale down to range [0, 1)
    double gamma = beta / (2 * PI);

    // and now avoid cyclic directions
    int d = (int(gamma * DIR_COUNT) + DIR_COUNT) % DIR_COUNT;
    
    return static_cast<Direction>(d);
}

Direction PointsDirection(const Point &lhs, const Point &rhs)
{
    return RadiansToDirection(atan2(lhs.x - rhs.x, lhs.y - rhs.y));
}

bool operator==(const Direction &lhs, const Direction &rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
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

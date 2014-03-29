#include "direction.h"

#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

namespace
{

    const int DirCount = 8;

    Direction Rotated(const Direction &dir, int times)
    {
        return Direction(
            (static_cast<int>(dir) + times + DirCount) % DirCount);
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

int LeftRotates(const Direction &lhs, const Direction &rhs)
{
    return (static_cast<int>(lhs) - static_cast<int>(rhs) + DirCount) % DirCount;
}

int RightRotates(const Direction &lhs, const Direction &rhs)
{
    return LeftRotates(rhs, lhs);
}

int MinRotates(const Direction &lhs, const Direction &rhs)
{
    return std::min(LeftRotates(lhs, rhs), RightRotates(lhs, rhs));
}

Direction RadiansToDirection(double angle)
{
    const static double PI = 3.141593;

    // Avoid negative angles
    double alpha = fmod(2 * PI + angle, 2 * PI);

    // Rotate by half of the direction's angle
    double beta = (alpha - 1.0f / (2 * DirCount));

    // scale down to range [0, 1)
    double gamma = beta / (2 * PI);

    // and now avoid cyclic directions
    int d = (int(gamma * DirCount) + DirCount) % DirCount;
    
    return static_cast<Direction>(d);
}

Direction PointsDirection(const Point &lhs, const Point &rhs)
{
    return RadiansToDirection(atan2(lhs.x - rhs.x, lhs.y - rhs.y));
}

Direction GetOppositeDirection(const Direction &dir)
{    
    switch(dir) {
    case Direction::East: return Direction::West;
    case Direction::NorthEast: return Direction::SouthWest;
    case Direction::North: return Direction::South;
    case Direction::NorthWest: return Direction::SouthEast;
    case Direction::West: return Direction::East;
    case Direction::SouthWest: return Direction::NorthEast;
    case Direction::South: return Direction::North;
    case Direction::SouthEast: return Direction::NorthWest;
    }

    throw std::runtime_error("Wrong direction");
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

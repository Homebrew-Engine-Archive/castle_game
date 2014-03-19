#ifndef DIRECTION_H_
#define DIRECTION_H_

#include <iosfwd>
#include "point.h"

/**
 * Just regular counter-clockwise directions.
 */
enum class Direction : int {
    East,
    NorthEast,
    North,
    NorthWest,
    West,
    SouthWest,
    South,
    SouthEast
};

Direction RotatedLeft(const Direction &dir, int times = 1);
Direction RotatedRight(const Direction &dir, int times = 1);

int LeftAngle(const Direction &lhs, const Direction &rhs);
int RightAngle(const Direction &lhs, const Direction &rhs);

int MinAngle(const Direction &lhs, const Direction &rhs);

Direction RadiansToDirection(double angle);
Direction PointsDirection(const Point &lhs, const Point &rhs);

bool operator==(const Direction &lhs, const Direction &rhs);

std::ostream &operator<<(std::ostream &out, const Direction &dir);

#endif

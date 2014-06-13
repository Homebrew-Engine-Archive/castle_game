#ifndef DIRECTION_H_
#define DIRECTION_H_

#include <vector>
#include <iosfwd>

#include <game/modulo.h>

class Point;

namespace core
{
    constexpr int MaxDirCount = 8;

    /** Just regular counter-clockwise directions.
     **/
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

    constexpr Direction Rotated(Direction dir, int times)
    {
        return static_cast<Direction>(
            core::Mod(
                static_cast<int>(dir) + times, MaxDirCount));
    }

    constexpr Direction RotatedCCW(Direction dir)
    {
        return Rotated(dir, -1);
    }

    constexpr Direction RotatedCW(Direction dir)
    {
        return Rotated(dir, 1);
    }

    constexpr int LeftRotates(Direction lhs, Direction rhs)
    {
        return core::Mod(static_cast<int>(lhs) - static_cast<int>(rhs), MaxDirCount);
    }

    constexpr int RightRotates(Direction lhs, Direction rhs)
    {
        return LeftRotates(rhs, lhs);
    }

    constexpr int MinRotates(Direction lhs, Direction rhs)
    {
        return (LeftRotates(lhs, rhs) < RightRotates(lhs, rhs)
                ? LeftRotates(lhs, rhs)
                : RightRotates(lhs, rhs));
    }

    constexpr Direction GetOppositeDirection(Direction dir)
    {
        return Rotated(dir, MaxDirCount / 2);
    }

    /**
     * \brief It maps regular 2D-plane on Direction.
     * \note The angle of 0 points on Direction::East
     */
    Direction RadiansToDirection(double angle);

    double DirectionToRadians(Direction dir);
    
    Direction PointsDirection(const Point &lhs, const Point &rhs);

    using DirectionSet = std::vector<Direction>;

    const DirectionSet EightDirs = DirectionSet {
        Direction::East,
        Direction::NorthEast,
        Direction::North,
        Direction::NorthWest,
        Direction::West,
        Direction::SouthWest,
        Direction::South,
        Direction::SouthEast
    };
    
    const DirectionSet FourDirs = DirectionSet {
        Direction::East,
        Direction::North,
        Direction::West,
        Direction::South
    };

    const DirectionSet SingleDir = DirectionSet {
        Direction::South
    };

    Direction ClosestDirection(const DirectionSet &set, const Direction &dir);

    std::ostream &operator<<(std::ostream &out, const Direction &dir);
} // namespace core

#endif

#ifndef DIRECTION_H_
#define DIRECTION_H_

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

    constexpr Direction DirFromNum(int num)
    {
        return static_cast<Direction>(num);
    }

    constexpr int DirToNum(Direction dir)
    {
        return core::Mod(static_cast<int>(dir), MaxDirCount);
    }
    
    constexpr Direction Rotated(Direction dir, int times)
    {
        return DirFromNum(
            core::Mod(
                DirToNum(dir) + times, MaxDirCount));
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
        return core::Mod(DirToNum(lhs) - DirToNum(rhs), MaxDirCount);
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

    std::ostream &operator<<(std::ostream &out, const Direction &dir);
} // namespace core

#endif

#ifndef LINE_H_
#define LINE_H_

#include <game/point.h>

struct Line
{
    Point p1;
    Point p2;

    constexpr Line()
        : p1(), p2()
        {}
    
    constexpr Line(const Point &a, const Point &b)
        : p1(a), p2(b)
        {}

    constexpr Line(int x1, int y1, int x2, int y2)
        : p1(x1, y1), p2(x2, y2)
        {}
};

constexpr Line Translated(const Line &line, const Point &offset)
{
    return Line(line.p1 + offset, line.p2 + offset);
}

constexpr bool operator==(const Line &L, const Line &R)
{
    return (L.p1 == R.p1) && (L.p2 == R.p2);
}

constexpr bool operator!=(const Line &L, const Line &R)
{
    return (L.p1 != R.p1) || (L.p2 != R.p2);
}

#endif // LINE_H_

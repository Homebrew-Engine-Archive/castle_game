#ifndef POINT_H_
#define POINT_H_

#include <iosfwd>

/**
 * Reimplement SDL_Point just for little dependency reduction.
 */
struct Point
{
    int x;
    int y;
    
    Point(int x, int y);
    Point();

    bool operator==(const Point &that) const;
    bool operator!=(const Point &that) const;
    
    bool operator<(const Point &that) const;
    bool operator<=(const Point &that) const;

    Point operator+(const Point &that) const;
    Point operator-(const Point &that) const;

    Point &operator+=(const Point &that);
    Point &operator-=(const Point &that);
};

double EuclidianDist(const Point &lhs, const Point &rhs);
int SqrEuclidianDist(const Point &lhs, const Point &rhs);

std::ostream &operator<<(std::ostream &out, const Point &pt);

#endif

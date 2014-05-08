#ifndef POINT_H_
#define POINT_H_

#include <iosfwd>

struct SDL_Point;

struct Point
{
    int x;
    int y;
    
    Point(int x, int y);
    Point();

    Point& operator+=(const Point &that);
    Point& operator-=(const Point &that);

    operator SDL_Point() const;
};

double EuclidianDist(const Point &lhs, const Point &rhs);
int ManhattanDist(const Point &lhs, const Point &rhs);

bool operator==(const Point &lhs, const Point &rhs);
bool operator!=(const Point &lhs, const Point &rhs);    
bool operator<(const Point &lhs, const Point &rhs);
bool operator<=(const Point &lhs, const Point &rhs);
bool operator>(const Point &lhs, const Point &rhs);
bool operator>=(const Point &lhs, const Point &rhs);

Point operator-(const Point &lhs, const Point &rhs);
Point operator+(const Point &lhs, const Point &rhs);
Point operator*(const Point &lhs, const Point &rhs);
Point operator/(const Point &lhs, const Point &rhs);

Point operator-(const Point &pt, int x);
Point operator+(const Point &pt, int x);
Point operator*(const Point &pt, int x);
Point operator/(const Point &pt, int x);

std::ostream &operator<<(std::ostream &out, const Point &pt);

#endif

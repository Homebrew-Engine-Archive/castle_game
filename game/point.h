#ifndef POINT_H_
#define POINT_H_

#include <iosfwd>
#include <SDL.h>

class Point : public SDL_Point
{
public:
    constexpr Point()
        : Point(0, 0) {}
    
    constexpr Point(int x, int y)
        : SDL_Point {x, y} {}

    Point& operator+=(const SDL_Point &that);
    Point& operator-=(const SDL_Point &that);
    Point& operator*=(const SDL_Point &that);
    Point& operator/=(const SDL_Point &that);

    template<class T> Point& operator+=(const T v) {
        x += v;
        y += v;
        return *this;
    }
    
    template<class T> Point& operator-=(const T v) {
        x -= v;
        y -= v;
        return *this;
    }
    
    template<class T> Point& operator*=(const T v) {
        x *= v;
        y *= v;
        return *this;
    }
    
    template<class T> Point& operator/=(const T v) {
        x /= v;
        y /= v;
        return *this;
    }
};

float Hypot(const SDL_Point &lhs, const SDL_Point &rhs);
int Manhattan(const SDL_Point &lhs, const SDL_Point &rhs);

bool operator==(const SDL_Point &lhs, const SDL_Point &rhs);
bool operator!=(const SDL_Point &lhs, const SDL_Point &rhs);    
bool operator<(const SDL_Point &lhs, const SDL_Point &rhs);
bool operator>(const SDL_Point &lhs, const SDL_Point &rhs);

Point operator-(const SDL_Point &lhs, const SDL_Point &rhs);
Point operator+(const SDL_Point &lhs, const SDL_Point &rhs);
Point operator*(const SDL_Point &lhs, const SDL_Point &rhs);
Point operator/(const SDL_Point &lhs, const SDL_Point &rhs);

Point operator-(const SDL_Point &pt, int x);
Point operator+(const SDL_Point &pt, int x);
Point operator*(const SDL_Point &pt, int x);
Point operator/(const SDL_Point &pt, int x);

Point TopLeft(const SDL_Rect &rect);
Point TopRight(const SDL_Rect &rect);
Point BottomLeft(const SDL_Rect &rect);
Point BottomRight(const SDL_Rect &rect);

Point RectCenter(const SDL_Rect &src);

bool PointInRect(const SDL_Rect &rect, int x, int y);
bool PointInRect(const SDL_Rect &rect, const SDL_Point &pt);

Point AlignPoint(const SDL_Rect &dst, double x, double y);
Point ShiftPoint(const SDL_Point &point, int x, int y);

std::ostream& operator<<(std::ostream &out, const SDL_Point &pt);

#endif

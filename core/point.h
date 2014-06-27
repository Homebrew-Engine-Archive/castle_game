#ifndef POINT_H_
#define POINT_H_

#include <cstdint>
#include <iosfwd>

#include <core/rect.h>

namespace core
{
    class Point
    {
    public:
        typedef int32_t coord_type;
        
        constexpr Point();
        constexpr Point(Point::coord_type x, Point::coord_type y);
    
        constexpr Point(const Point &that) = default;
        constexpr Point(Point&&) = default;
    
        Point& operator=(Point const&) = default;
        Point& operator=(Point&&) = default;
    
        Point& operator+=(coord_type v);
        Point& operator-=(coord_type v);
        Point& operator*=(coord_type v);
        Point& operator/=(coord_type v);
        Point& operator+=(const Point &that);
        Point& operator-=(const Point &that);
        Point& operator*=(const Point &that);
        Point& operator/=(const Point &that);

        constexpr inline coord_type X() const;
        constexpr inline coord_type Y() const;

        inline void SetX(coord_type x);
        inline void SetY(coord_type y);
    private:
        coord_type mX;
        coord_type mY;
    };

    constexpr Point::Point(Point::coord_type x, Point::coord_type y)
        : mX(x)
        , mY(y)
    {
    }

    constexpr Point::Point()
        : Point(Point::coord_type(), Point::coord_type())
    {
    }

    constexpr inline Point::coord_type Point::X() const
    {
        return mX;
    }

    constexpr inline Point::coord_type Point::Y() const
    {
        return mY;
    }

    inline void Point::SetX(Point::coord_type x)
    {
        mX = x;
    }
    
    inline void Point::SetY(Point::coord_type y)
    {
        mY = y;
    }
    
    constexpr Point operator-(const Point &point)
    {
        return Point(-point.X(), -point.Y());
    }

    constexpr bool operator==(const Point &lhs, const Point &rhs)
    {
        return (lhs.X() == rhs.X()) && (lhs.Y() == rhs.Y());
    }

    constexpr bool operator!=(const Point &lhs, const Point &rhs)
    {
        return (lhs.X() != rhs.X()) || (lhs.Y() != rhs.Y());
    }

    constexpr bool operator<(const Point &lhs, const Point &rhs)
    {
        return (lhs.X() < rhs.X()) || (lhs.X() == rhs.X() && lhs.Y() < rhs.Y());
    }

    constexpr bool operator>(const Point &lhs, const Point &rhs)
    {
        return (lhs.X() > rhs.X()) || (lhs.X() == rhs.X() && lhs.Y() > rhs.Y());
    }

    constexpr Point operator+(const Point &lhs, const Point &rhs)
    {
        return Point(lhs.X() + rhs.X(), lhs.Y() + rhs.Y());
    }

    constexpr Point operator-(const Point &lhs, const Point &rhs)
    {
        return Point(lhs.X() - rhs.X(), lhs.Y() - rhs.Y());
    }

    constexpr Point operator*(const Point &lhs, const Point &rhs)
    {
        return Point(lhs.X() * rhs.X(), lhs.Y() * rhs.Y());
    }

    constexpr Point operator/(const Point &lhs, const Point &rhs)
    {
        return Point(lhs.X() / rhs.X(), lhs.Y() / rhs.Y());
    }

    constexpr Point operator+(const Point &pt, Point::coord_type v)
    {
        return Point(pt.X() + v, pt.Y() + v);
    }

    constexpr Point operator-(const Point &pt, Point::coord_type v)
    {
        return Point(pt.X() - v, pt.Y() - v);
    }

    constexpr Point operator*(const Point &pt, Point::coord_type v)
    {
        return Point(pt.X() * v, pt.Y() * v);
    }

    constexpr Point operator/(const Point &pt, Point::coord_type v)
    {
        return Point(pt.X() / v, pt.Y() / v);
    }

    constexpr Point TopLeft(const Rect &rect)
    {
        return Point(rect.X1(), rect.Y1());
    }

    constexpr Point TopRight(const Rect &rect)
    {
        return Point(rect.X2(), rect.Y1());
    }

    constexpr Point BottomLeft(const Rect &rect)
    {
        return Point(rect.X1(), rect.Y2());
    }

    constexpr Point BottomRight(const Rect &rect)
    {
        return Point(rect.X2(), rect.Y2());
    }

    constexpr Point RectCenter(const Rect &rect)
    {
        return Point(rect.X() + rect.Width() / 2,
                     rect.Y() + rect.Height() / 2);
    }

    constexpr Point AlignPoint(const Rect &rt, double x, double y)
    {
        return Point(rt.X() + rt.Width() * (x + 1.0) / 2.0,
                     rt.Y() + rt.Height() * (y + 1.0) / 2.0);
    }

    constexpr bool PointInRect(const Rect &rect, const Point &pt)
    {
        return (pt.X() >= rect.X1())
            && (pt.Y() >= rect.Y1())
            && (pt.X() <= rect.X2())
            && (pt.Y() <= rect.Y2());
    }

    constexpr Point ClipToRect(const Rect &rect, const Point &pt)
    {
        return Point((pt.X() - rect.X()) % rect.Width(), (pt.Y() - rect.Y()) % rect.Height());
    }

    float Hypot(const Point &lhs, const Point &rhs);
    int Manhattan(const Point &lhs, const Point &rhs);

    std::ostream& operator<<(std::ostream &out, const Point &pt);
}

#endif

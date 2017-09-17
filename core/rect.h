#ifndef RECT_H_
#define RECT_H_

#include <cstdint>
#include <iosfwd>

#include <core/minmax.h>
#include <core/interval.h>

namespace core
{
    class Point;
}

namespace core
{
    class Rect
    {
    public:
        typedef int coord_type;
        
        constexpr Rect();
        constexpr Rect(coord_type w, coord_type h);
        constexpr Rect(coord_type x, coord_type y, coord_type w, coord_type h);
        explicit Rect(const core::Point &wh);    
        Rect(const core::Point &lt, const core::Point &br);
    
        constexpr Rect(const Rect &that) = default;
        constexpr Rect(Rect&&) = default;

        Rect& operator=(const Rect &that) = default;
        Rect& operator=(Rect&&) = default;

        constexpr inline coord_type X() const;
        constexpr inline coord_type Y() const;
        constexpr inline coord_type Width() const;
        constexpr inline coord_type Height() const;
        constexpr inline coord_type X1() const;
        constexpr inline coord_type Y1() const;
        constexpr inline coord_type X2() const;
        constexpr inline coord_type Y2() const;
        inline void SetX(coord_type x);
        inline void SetY(coord_type y);
        inline void SetWidth(coord_type width);
        inline void SetHeight(coord_type height);
        
    private:
        coord_type mX;
        coord_type mY;
        coord_type mWidth;
        coord_type mHeight;
    };

    constexpr Rect::Rect(Rect::coord_type x, Rect::coord_type y, Rect::coord_type width, Rect::coord_type height)
        : mX(x)
        , mY(y)
        , mWidth(width)
        , mHeight(height)
    {
    }

    constexpr Rect::Rect(Rect::coord_type width, Rect::coord_type height)
        : Rect(Rect::coord_type(), Rect::coord_type(), width, height)
    {
    }

    constexpr Rect::Rect()
        : Rect(Rect::coord_type(), Rect::coord_type(), Rect::coord_type(), Rect::coord_type())
    {
    }

    constexpr bool operator==(const Rect &lhs, const Rect &rhs)
    {
        return (lhs.X() == rhs.X())
            && (lhs.Y() == rhs.Y())
            && (lhs.Width() == rhs.Width())
            && (lhs.Height() == rhs.Height());
    }

    constexpr bool operator!=(const Rect &lhs, const Rect &rhs)
    {
        return (lhs.X() != rhs.X())
            || (lhs.Y() != rhs.Y())
            || (lhs.Width() != rhs.Width())
            || (lhs.Height() != rhs.Height());
    }

    constexpr bool Intersects(const Rect &lhs, const Rect &rhs)
    {
        return (lhs.X() <= rhs.X() + rhs.Width())
            && (rhs.X() <= lhs.X() + lhs.Width())
            && (lhs.Y() < rhs.Y() + rhs.Height())
            && (rhs.Y() < lhs.Y() + lhs.Height());
    }

    constexpr Rect Translated(const Rect &rect, Rect::coord_type dx, Rect::coord_type dy)
    {
        return Rect(rect.X() + dx, rect.Y() + dy, rect.Width(), rect.Height());
    }

    constexpr bool RectEmpty(const Rect &rect)
    {
        return ((rect.Width() <= 0) || (rect.Height() <= 0));
    }

    constexpr Rect ChopBottom(const Rect &rect, Rect::coord_type howmany)
    {
        return Rect(rect.X(),
                    rect.Y(),
                    rect.Width(),
                    Max(0, rect.Height() - howmany));
    }

    constexpr Rect ChopTop(const Rect &rect, Rect::coord_type howmany)
    {
        return Rect(rect.X(),
                    rect.Y() + Min(howmany, rect.Height()),
                    rect.Width(),
                    Max(0, rect.Height() - howmany));
    }

    constexpr Rect ChopLeft(const Rect &rect, Rect::coord_type howmany)
    {
        return Rect(rect.X() + Min(howmany, rect.Width()),
                    rect.Y(),
                    Max(0, rect.Width()),
                    rect.Height());
    }

    constexpr Rect ChopRight(const Rect &rect, Rect::coord_type howmany)
    {
        return Rect(rect.X(),
                    rect.Y(),
                    Max(0, rect.Width()),
                    rect.Height());
    }

    template<class T>
    constexpr const Rect CombineRect(const Interval<T> &axisX, const Interval<T> &axisY)
    {
        return Rect(axisX.min, axisY.min,
                    axisX.max - axisX.min,
                    axisY.max - axisY.min);
    }

    constexpr const Interval<Rect::coord_type> AxisX(const Rect &rect)
    {
        return Interval<Rect::coord_type>(rect.X(), rect.X() + rect.Width());
    }

    constexpr const Interval<Rect::coord_type> AxisY(const Rect &rect)
    {
        return Interval<Rect::coord_type>(rect.Y(), rect.Y() + rect.Height());
    }

    constexpr const Rect Union(const Rect &lhs, const Rect &rhs)
    {
        return CombineRect(
            Union(AxisX(lhs), AxisX(rhs)),
            Union(AxisY(lhs), AxisY(rhs)));
    }

    constexpr const Rect Intersection(const Rect &lhs, const Rect &rhs)
    {
        return CombineRect(
            Intersection(AxisX(lhs), AxisX(rhs)),
            Intersection(AxisY(lhs), AxisY(rhs)));
    }
    
    inline void Rect::SetX(Rect::coord_type x)
    {
        mX = x;
    }
    
    inline void Rect::SetY(Rect::coord_type y)
    {
        mY = y;
    }
    
    inline void Rect::SetWidth(Rect::coord_type width)
    {
        mWidth = width;
    }
    
    inline void Rect::SetHeight(Rect::coord_type height)
    {
        mHeight = height;
    }
    
    constexpr inline Rect::coord_type Rect::X() const
    {
        return mX;
    }
    
    constexpr inline Rect::coord_type Rect::Y() const
    {
        return mY;
    }
    
    constexpr inline Rect::coord_type Rect::Width() const
    {
        return mWidth;
    }
    
    constexpr inline Rect::coord_type Rect::Height() const
    {
        return mHeight;
    }
    
    constexpr inline Rect::coord_type Rect::X1() const
    {
        return X();
    }
    
    constexpr inline Rect::coord_type Rect::Y1() const
    {
        return Y();
    }
    
    constexpr inline Rect::coord_type Rect::X2() const
    {
        return X() + Width();
    }
    
    constexpr inline Rect::coord_type Rect::Y2() const
    {
        return Y() + Height();
    }
        
    std::ostream& operator<<(std::ostream &out, const Rect &rect);

    
    /**
       \brief Calculate layout rect for src in dst

       One assumed that src is smaller and dst is larger.
       The area of dst is mapped onto two-dimensional plane with bounds [-1..1] by x and y.

       \param src       Inner rect (only width and height matter).
       \param dst       Outer rect.
       \param x         Relative x-pos of src center
       \param y         Relative y-pos of src center

    **/
    const Rect PutIn(const Rect &src, const Rect &dst, double x, double y);
    
    const Rect PadIn(const Rect &src, Rect::coord_type pad);
    const Rect PadOut(const Rect &src, Rect::coord_type pad);
    const Rect Normalized(const Rect &src);
}

#endif // RECT_H_

#include "rect.h"

#include <iostream>

#include <core/clamp.h>
#include <core/minmax.h>
#include <core/point.h>

namespace core
{
    Rect::Rect(const core::Point &size)
        : Rect(size.X(), size.Y())
    {
    }

    Rect::Rect(const core::Point &lt, const core::Point &br)
        : Rect(lt.X(), lt.Y(), br.X() - lt.X(), br.Y() - lt.Y())
    {
    }

    const Rect Normalized(const Rect &rect)
    {
        return Rect(Min(rect.X1(), rect.X2()),
                    Min(rect.Y1(), rect.Y2()),
                    Max(rect.X1(), rect.X2()),
                    Max(rect.Y1(), rect.Y2()));
    }

    const Rect PutIn(const Rect &src, const Rect &dst, double x, double y)
    {
        const Rect::coord_type xcenter = 2 * dst.X2();
        const Rect::coord_type ycenter = 2 * dst.Y2();

        const Rect::coord_type xspace = Max(0, dst.Width() - src.Width());
        const Rect::coord_type yspace = Max(0, dst.Height() - src.Height());

        const Rect::coord_type xpos = x * xspace;
        const Rect::coord_type ypos = y * yspace;

        return Rect(
            (xcenter + xpos - src.Width()) / 2,
            (ycenter + ypos - src.Height()) / 2,
            src.Width(),
            src.Height());
    }

    const Rect PadOut(const Rect &src, Rect::coord_type pad)
    {
        return Rect(src.X1() - pad,
                    src.Y1() - pad,
                    src.X2() + pad,
                    src.Y2() + pad);
    }

    const Rect PadIn(const Rect &src, Rect::coord_type pad)
    {
        return Rect(
            Min(src.X1() + pad, src.X2() - pad),
            Min(src.Y1() + pad, src.Y2() - pad),
            Max(src.X1() + pad, src.X2() - pad),
            Max(src.X1() + pad, src.Y2() - pad));
    }

    std::ostream& operator<<(std::ostream &out, const Rect &rect)
    {
        out << rect.Width() << 'x' << rect.Height();
        if(rect.X() >= 0)
            out << '+';
        out << rect.X();
        if(rect.Y() >= 0)
            out << '+';
        out << rect.Y();
        return out;
    }
}

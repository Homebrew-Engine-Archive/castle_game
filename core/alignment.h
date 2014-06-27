#ifndef ALIGNMENT_H_
#define ALIGNMENT_H_

#include <core/interval.h>

namespace core
{
    enum class Alignment : int
    {
        Min,
        Center,
        Max,
        Expanded
    };

    template<class T>
    const Interval<T> AlignIntervals(const Interval<T> &src, const Interval<T> &dst, Alignment align)
    {
        switch(align) {
        case Alignment::Min:
            return Interval<T>(dst.min, dst.min + (src.max - src.min));
        case Alignment::Max:
            return Interval<T>(dst.max - (src.max - src.min), dst.max);
        case Alignment::Center:
            return Interval<T>(((dst.min + dst.max) - (src.max - src.min)) / 2,
                               ((dst.min + dst.max) + (src.max - src.min)) / 2);
        case Alignment::Expanded:
            return dst;
        default:
            return src;
        }
    }

}

#endif // ALIGNMENT_H_

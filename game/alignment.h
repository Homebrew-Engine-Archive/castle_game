#ifndef ALIGNMENT_H_
#define ALIGNMENT_H_

#include <game/range.h>

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
    Range<T> AlignRange(const Range<T> &src, const Range<T> &dst, Alignment align)
    {
        switch(align) {
        case Alignment::Min:
            return Range<T>(dst.min, dst.min + (src.max - src.min));
        case Alignment::Max:
            return Range<T>(dst.max - (src.max - src.min), dst.max);
        case Alignment::Center:
            return Range<T>(((dst.min + dst.max) - (src.max - src.min)) / 2,
                            ((dst.min + dst.max) + (src.max - src.min)) / 2);
        case Alignment::Expanded:
            return dst;
        default:
            return src;
        }
    }

}

#endif // ALIGNMENT_H_

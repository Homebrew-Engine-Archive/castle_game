#ifndef SIZE_H_
#define SIZE_H_

#include <core/rect.h>
#include <core/point.h>

namespace core
{
    template<class T>
    class SizeT
    {
    public:
        typedef T coord_type;
        constexpr inline explicit SizeT(coord_type w = coord_type(), coord_type h = coord_type());
        constexpr inline coord_type Width() const;
        constexpr inline coord_type Height() const;
        inline void SetWidth(coord_type width);
        inline void SetHeight(coord_type height);
        
    private:
        coord_type mWidth;
        coord_type mHeight;
    };

    template<class T>
    constexpr inline SizeT<T>::SizeT(typename SizeT<T>::coord_type w, typename SizeT<T>::coord_type h)
        : mWidth(w)
        , mHeight(h)
    {
    }

    template<class T>
    constexpr inline
    typename SizeT<T>::coord_type SizeT<T>::Width() const
    {
        return mWidth;
    }

    template<class T>
    constexpr inline
    typename SizeT<T>::coord_type SizeT<T>::Height() const
    {
        return mHeight;
    }

    template<class T>
    inline void SizeT<T>::SetWidth(typename SizeT<T>::coord_type width)
    {
        mWidth = width;
    }

    template<class T>
    inline void SizeT<T>::SetHeight(typename SizeT<T>::coord_type height)
    {
        mHeight = height;
    }
    
    /** "Specialize" SizeT template for int and float using different names **/
    struct Size : public SizeT<int>
    {
        /** Inheriting constructors is a C++11 feature
            and it requires GCC-4.8 or newer
        **/
        using SizeT::SizeT;
    };

    struct SizeF : public SizeT<float>
    {
        using SizeT::SizeT;
    };

    constexpr Size RectSize(const Rect &rect)
    {
        return Size(rect.Width(), rect.Height());
    }

    constexpr Size FromPoint(const Point &point)
    {
        return Size(point.X(), point.Y());
    }
}

#endif // SIZE_H_

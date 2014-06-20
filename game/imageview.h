#ifndef IMAGEVIEW_H_
#define IMAGEVIEW_H_

#include <game/image.h>

namespace core
{
    class Rect;
}

namespace castle
{
/**
   \brief Region-Of-Interest of surface object
 
   It is a surface like object which shares some memory with
   it's parent surface object. Exactly like OpenCV's Mat class can do .
   
   \note Deallocation of parent object isn't invalidate such kind of object.

   It holds reference to the parent surface so neither this nor parent
   surface doesn't intent deallocation of each other.

   \todo This class is not intended to work together with RLE accel. Can we deal with it?
**/

    class ImageView final
    {
        Image mImage;
        const Image &mParentRef;
    
      public:
        ImageView(const Image &src, const core::Rect &clip);
        ImageView(Image &src, const core::Rect &clip);

        inline Image& GetView();
        inline Image const& GetView() const;
    };

    inline Image& ImageView::GetView()
    {
        return mImage;
    }

    inline Image const& ImageView::GetView() const
    {
        return mImage;
    }
}

#endif // IMAGEVIEW_H_

#ifndef SURFACE_VIEW_H_
#define SURFACE_VIEW_H_

#include <game/surface.h>

class Rect;

/**
   \brief Region-Of-Interest of surface object
 
   It is a surface like object which shares some memory with
   it's parent surface object. Exactly like OpenCV's Mat class can do .
   
   \note Deallocation of parent object isn't invalidate such kind of object.

   It holds reference to the parent surface so neither this nor parent
   surface doesn't intent deallocation of each other.

   \todo This class is not intended to work together with RLE accel. Can we deal with it?
**/

class SurfaceView final
{
    Surface mSurface;
    const Surface &mParentRef;
    
public:
    SurfaceView(const Surface &src, const Rect &clip);
    SurfaceView(Surface &src, const Rect &clip);

    inline Surface& View()
    {
        return mSurface;
    }
    
    inline Surface const& View() const
    {
        return mSurface;
    }
};

#endif // SURFACE_VIEW_H_
